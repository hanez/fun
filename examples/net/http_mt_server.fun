#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-03-26
 */

#include <io/socket.fun>
#include <io/thread.fun>

// Simple multi-threaded HTTP server (thread-per-connection)
//
// Features:
// - Blocking accept() in the main thread
// - Each client is handled by a separate Fun thread
// - Sends a minimal HTTP 200 response with a small HTML body
//
// Usage:
//   ./examples/net/http_mt_server.fun
//   Open http://127.0.0.1:8089/ in the browser

PORT = 8080
BACKLOG = 128

srv = TcpServer(PORT, BACKLOG)
if (srv.listen() <= 0)
  print("HTTP MT server: listen failed on :" + to_string(PORT))
  return 0
print("HTTP MT server on :" + to_string(PORT))

fun handle_client(fd)
  req = sock_recv(fd, 8192)
  if (len(req) == 0)
    sock_close(fd)
    return 0

  body = "<html><body><h1>Hello from Fun multi-threaded server</h1></body></html>"
  b = to_string(body)
  resp = "HTTP/1.1 200 OK\r\n"
  resp = resp + "Content-Type: text/html; charset=utf-8\r\n"
  resp = resp + "Content-Length: " + to_string(len(b)) + "\r\n"
  resp = resp + "Connection: close\r\n\r\n" + b
  sock_send(fd, resp)
  sock_close(fd)
  return 1

th = Thread()

// Accept loop: spawn a thread per client
while true
  fd = srv.accept()
  if (fd > 0)
    _ = th.spawn(handle_client, fd)

/* Expected output (on start):
HTTP MT server on :8080

Then open http://127.0.0.1:8089/ in a browser; it will render:
<html><body><h1>Hello from Fun multi-threaded server</h1></body></html>
*/
