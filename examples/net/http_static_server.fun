#!/usr/bin/env fun

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

#include <io/socket.fun>

port = 8088
srv = TcpServer(port, 10)
if (srv.listen() <= 0)
  print("HTTP static server: listen failed on :" + to_string(port))
  return 0
print("HTTP static server on :" + to_string(port))
while true
  fd = srv.accept()
  if (fd > 0)
    _ = sock_recv(fd, 4096)  // ignore request
    body = "<html><body><h1>Hello from Fun static server</h1></body></html>"
    b = to_string(body)
    resp = "HTTP/1.1 200 OK\r\n"
    resp = resp + "Content-Type: text/html; charset=utf-8\r\n"
    resp = resp + "Content-Length: " + to_string(len(b)) + "\r\n"
    resp = resp + "Connection: close\r\n\r\n" + b
    sock_send(fd, resp)
    sock_close(fd)

/* Expected output (on start):
HTTP static server on :8088

Then open http://127.0.0.1:8088/ in a browser; it will render:
<html><body><h1>Hello from Fun static server</h1></body></html>
*/
