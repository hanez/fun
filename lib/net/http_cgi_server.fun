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

// Blocking HTTP server with CGI support

#include <io/socket.fun>
#include <strings.fun>
#include <net/cgi.fun>

class HTTPCGIServer(number port)
  fun _construct(this, port)
    this.port = port
    this.srv = TcpServer(port, 10)
    this.htdocs = "./"

  fun set_htdocs(this, path)
    this.htdocs = to_string(path)

  fun start(this)
    if (this.srv.listen() <= 0)
      print("HTTP CGI Server: failed to listen on port " + to_string(this.port))
      return 0
    print("HTTP CGI Server: serving " + this.htdocs + " on port " + to_string(this.port))
    while true
      fd = this.srv.accept()
      if (fd > 0)
        this.handle_client(fd)
    return 1

  // Safe header getter: returns string or empty string when missing/invalid
  fun _hget(this, hdrs, key)
    if (typeof(hdrs) == "Map")
      v = hdrs[key]
      if (typeof(v) == "String")
        return v
      if (v != nil)
        // Coerce non-strings to string just in case
        return to_string(v)
    return ""

  fun _parse_headers(this, request)
    headers = {}
    lines = str_split(request, "\n")
    // str_split should return an Array; if not, bail out with empty headers
    if (!(typeof(lines) == "Array"))
      return headers
    i = 1  // start after request line
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

    // Robustly extract request line without relying on array indexing
    reqstr = to_string(request)
    // debug: mark start of handle_client
    // print("[DBG] handle_client: received request bytes=" + to_string(len(reqstr)))
    nl = find(reqstr, "\n")
    if (nl < 0)
      nl = find(reqstr, "\r")
    if (nl < 0)
      sock_close(fd)
      return 0
    reqline = str_trim(substr(reqstr, 0, nl))
    // print("[DBG] request line=\"" + reqline + "\"")

    parts = str_split(reqline, " ")
    // Guard against unexpected splitter behavior
    if (!(typeof(parts) == "Array") || len(parts) < 2)
      sock_close(fd)
      return 0
    // print("[DBG] parts ok, count=" + to_string(len(parts)) + ", t0=" + typeof(parts))

    method = str_trim(parts[0])
    target = str_trim(parts[1])
    // print("[DBG] method=" + method + ", target=" + target)

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
    // Parse headers; if anything goes wrong, fall back to an empty map
    headers = {}
    tmp_headers = this._parse_headers(request)
    if (typeof(tmp_headers) == "Map")
      headers = tmp_headers
    // print("[DBG] headers parsed, type=" + typeof(headers))

    // Extract POST body
    body = ""
    header_end = find(request, "\r\n\r\n")
    if (header_end >= 0)
      body = substr(request, header_end + 4, len(request) - header_end - 4)
    else
      header_end = find(request, "\n\n")
      if (header_end >= 0)
        body = substr(request, header_end + 2, len(request) - header_end - 2)

    // Serve CGI when file ends with .fun
    if (str_ends_with(path, ".fun"))
      // Build env vars according to CGI conventions.
      // Extract selected request headers safely (map lookups may yield nil).
      host = this._hget(headers, "HOST")
      if (len(host) == 0) host = "localhost"
      ua = this._hget(headers, "USER-AGENT")
      cookie = this._hget(headers, "COOKIE")
      ctype = this._hget(headers, "CONTENT-TYPE")
      clen = this._hget(headers, "CONTENT-LENGTH")

      envs = []
      // Ensure Fun stdlib is available to CGI child process
      funlib = env("FUN_LIB_DIR")
      if (len(funlib) == 0)
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
      exec = env("FUN_EXEC")
      if (len(exec) == 0)
        if (len(read_file("./build_debug/fun")) > 0)
          exec = "./build_debug/fun"
        else
          if (len(read_file("./build_release/fun")) > 0)
            exec = "./build_release/fun"
          else
            exec = "fun"

      // Run the Fun script as a CGI program
      cmd = join(envs, " ") + " " + exec + " " + file_path
      // print("[DBG] CGI exec: " + cmd)
      res = proc_run(cmd)

      // Be defensive: proc_run() may fail or return a non-map depending on platform
      if (res == nil || typeof(res) != "Map")
        print("[CGI] proc_run failed for: " + file_path)
        this._send(fd, 500, "Internal Server Error", "<h1>Failed to execute CGI</h1>")
        sock_close(fd)
        return 0
      
      // Safely extract fields from result map; guard each access
      out = ""
      code = 0
      tmp = nil
      if (typeof(res) == "Map")
        tmp = res["out"]
      if (tmp != nil)
        out = tmp
      tmpc = nil
      if (typeof(res) == "Map")
        tmpc = res["code"]
      if (tmpc != nil)
        code = tmpc

      if (len(out) == 0)
        if (to_string(code) != "0")
          this._send(fd, 500, "Internal Server Error", "<h1>CGI failed (exit " + to_string(code) + ")</h1>")
        else
          this._send(fd, 500, "Internal Server Error", "<h1>CGI produced no output</h1>")
      else
        this._send_cgi_response(fd, out)
    else
      // Static file
      content = read_file(file_path)
      // print("[DBG] static path: " + file_path + ", content_len=" + to_string(len(content)))
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
