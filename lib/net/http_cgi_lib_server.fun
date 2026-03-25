/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-03-25
 */

// Blocking HTTP server that uses Fun interpreter to execute .fun CGI files

#include <io/socket.fun>
#include <strings.fun>
#include <net/cgi.fun>

class HTTPCGILibServer(number port)
  fun _construct(this, port)
    this.port = port
    this.srv = TcpServer(port, 10)
    this.htdocs = "./"

  fun set_htdocs(this, path)
    this.htdocs = to_string(path)

  fun start(this)
    if (this.srv.listen() <= 0)
      print("HTTP CGI(lib) Server: failed to listen on port " + to_string(this.port))
      return 0
    print("HTTP CGI(lib) Server: serving " + this.htdocs + " on port " + to_string(this.port))
    while true
      fd = this.srv.accept()
      if (fd > 0)
        this.handle_client(fd)
    return 1

  fun _parse_headers(this, request)
    headers = {}
    lines = str_split(request, "\n")
    i = 1
    while (i < len(lines))
      ln = str_trim(lines[i])
      if (len(ln) == 0)
        break
      colon = find(ln, ":")
      if (colon > 0)
        k = str_to_upper(str_trim(substr(ln, 0, colon)))
        v = str_trim(substr(ln, colon + 1, len(ln) - colon - 1))
        headers[k] = v
      i = i + 1
    return headers

  fun handle_client(this, fd)
    request = sock_recv(fd, 65536)
    if (len(request) == 0)
      sock_close(fd)
      return 0

    lines = str_split(request, "\n")
    if (len(lines) == 0)
      sock_close(fd)
      return 0

    reqline = str_trim(lines[0])
    parts = str_split(reqline, " ")
    if (len(parts) < 2)
      sock_close(fd)
      return 0

    method = str_trim(parts[0])
    target = str_trim(parts[1])

    // Split query string
    path = target
    query = ""
    q = find(target, "?")
    if (q >= 0)
      path = substr(target, 0, q)
      query = substr(target, q + 1, len(target) - q - 1)

    if (path == "/")
      path = "/index.html"

    file_path = this.htdocs + path

    // Headers and body
    headers = this._parse_headers(request)

    // Extract POST body
    body = ""
    header_end = find(request, "\r\n\r\n")
    if (header_end >= 0)
      body = substr(request, header_end + 4, len(request) - header_end - 4)
    else
      header_end = find(request, "\n\n")
      if (header_end >= 0)
        body = substr(request, header_end + 2, len(request) - header_end - 2)

    // Treat any .fun under htdocs as a CGI script executed by the Fun interpreter
    if (str_ends_with(path, ".fun"))
      // Build env vars according to CGI conventions
      host = headers["HOST"]
      if (len(host) == 0) host = "localhost"
      ua = headers["USER-AGENT"]
      cookie = headers["COOKIE"]
      ctype = headers["CONTENT-TYPE"]
      clen = headers["CONTENT-LENGTH"]

      envs = []
      // Ensure Fun stdlib is available to CGI child process
      funlib = env("FUN_LIB_DIR")
      if (len(funlib) == 0)
        // Best-effort default when running from project root
        funlib = "./lib"
      push(envs, "FUN_LIB_DIR='" + funlib + "'")
      push(envs, "REQUEST_METHOD='" + method + "'")
      push(envs, "QUERY_STRING='" + query + "'")
      push(envs, "SCRIPT_NAME='" + path + "'")
      push(envs, "PATH_INFO='" + path + "'")
      push(envs, "SERVER_NAME='" + host + "'")
      push(envs, "SERVER_PORT='" + to_string(this.port) + "'")
      push(envs, "SERVER_PROTOCOL='HTTP/1.1'")
      push(envs, "HTTP_HOST='" + host + "'")
      if (len(ua) > 0)
        push(envs, "HTTP_USER_AGENT='" + ua + "'")
      if (len(cookie) > 0)
        push(envs, "HTTP_COOKIE='" + cookie + "'")
      if (len(ctype) > 0)
        push(envs, "CONTENT_TYPE='" + ctype + "'")
      if (len(clen) > 0)
        push(envs, "CONTENT_LENGTH='" + clen + "'")
      if (len(body) > 0)
        push(envs, "POST_DATA='" + body + "'")

      // Decide which Fun interpreter to use for the CGI child
      // Priority:
      // 1) $FUN_EXEC (explicit override)
      // 2) ./build_debug/fun (when running from project root)
      // 3) ./build_release/fun (release build)
      // 4) fun (from PATH)
      exec = env("FUN_EXEC")
      if (len(exec) == 0)
        // crude existence check using read_file; good enough here
        if (len(read_file("./build_debug/fun")) > 0)
          exec = "./build_debug/fun"
        else
          if (len(read_file("./build_release/fun")) > 0)
            exec = "./build_release/fun"
          else
            exec = "fun"

      // Run the Fun script as a CGI program
      cmd = join(envs, " ") + " " + exec + " " + file_path
      res = proc_run(cmd)
      out = res["out"]

      if (len(out) == 0)
        this._send(fd, 500, "Internal Server Error", "<h1>CGI produced no output</h1>")
      else
        this._send_cgi_response(fd, out)
    else
      // Static file
      content = read_file(file_path)
      if (len(content) > 0)
        this._send(fd, 200, "OK", content)
      else
        this._send(fd, 404, "Not Found", "<h1>404 Not Found</h1>")

    sock_close(fd)
    return 1

  fun _send(this, fd, code, text, body)
    b = to_string(body)
    resp = "HTTP/1.1 " + to_string(code) + " " + text + "\r\n"
    resp = resp + "Content-Type: text/html; charset=utf-8\r\n"
    resp = resp + "Content-Length: " + to_string(len(b)) + "\r\n"
    resp = resp + "Connection: close\r\n\r\n" + b
    sock_send(fd, resp)

  // Translate CGI output (headers + body) into a proper HTTP/1.1 response
  fun _send_cgi_response(this, fd, raw)
    tmpcgi = CGI()
    resp = tmpcgi.cgi_to_http_response(raw)
    sock_send(fd, resp)
