/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-12-28
 */

#include <io/socket.fun>
#include <strings.fun>

class HTTPServer(number port)
  fun _construct(this, port)
    this.port = port
    srv = TcpServer(port, 10)
    this.server = srv
    this.htdocs = "./"

  fun set_htdocs(this, path)
    this.htdocs = to_string(path)

  fun start(this)
    if (this.server.listen() <= 0)
      print("HTTPServer: failed to listen on port " + to_string(this.port))
      return 0
    print("HTTPServer: serving " + this.htdocs + " on port " + to_string(this.port))
    while true
      client_fd = this.server.accept()
      if (client_fd > 0)
        this.handle_client(client_fd)
    return 1

  fun handle_client(this, fd)
    request = sock_recv(fd, 4096)
    if (len(request) == 0)
      sock_close(fd)
      return 0

    // Basic request parsing
    lines = str_split(request, "\n")
    if (len(lines) == 0)
      sock_close(fd)
      return 0

    first_line = lines[0]
    parts = str_split(first_line, " ")
    if (len(parts) < 2)
      sock_close(fd)
      return 0

    method = str_trim(parts[0])
    full_path = str_trim(parts[1])

    path = full_path
    query_string = ""
    q_pos = find(full_path, "?")
    if (q_pos >= 0)
      path = substr(full_path, 0, q_pos)
      query_string = substr(full_path, q_pos + 1, len(full_path) - q_pos - 1)

    if (path == "/")
      path = "/index.html"

    file_path = this.htdocs + path

    post_data = ""
    if (method == "POST")
      // Find the end of headers (marked by \r\n\r\n or \n\n)
      header_end = find(request, "\r\n\r\n")
      if (header_end >= 0)
        post_data = substr(request, header_end + 4, len(request) - header_end - 4)
      else
        header_end = find(request, "\n\n")
        if (header_end >= 0)
          post_data = substr(request, header_end + 2, len(request) - header_end - 2)

    content = ""
    if (str_ends_with(path, ".fun"))
      env_vars = ""
      if (len(query_string) > 0)
        env_vars = "QUERY_STRING='" + query_string + "' "
      if (len(post_data) > 0)
        env_vars = env_vars + "POST_DATA='" + post_data + "' "
      
      res = proc_run(env_vars + " " + "fun" + " " + file_path)
      content = res["out"]
    else
      content = read_file(file_path)

    if (len(content) > 0)
      this.send_response(fd, 200, "OK", content)
    else
      this.send_response(fd, 404, "Not Found", "<h1>404 Not Found</h1>")

    sock_close(fd)
    return 1

  fun send_response(this, fd, status_code, status_text, body)
    resp = "HTTP/1.1 " + to_string(status_code) + " " + status_text + "\r\n"
    resp = resp + "Content-Type: text/html\r\n"
    resp = resp + "Content-Length: " + to_string(len(body)) + "\r\n"
    resp = resp + "Connection: close\r\n"
    resp = resp + "\r\n"
    resp = resp + body
    sock_send(fd, resp)
