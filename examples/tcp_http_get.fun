/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-04
 */

/*
 * Simple TCP client in Fun: fetches HTTP/1.0 from example.org
 * Requires network access. Demonstrates tcp_connect, sock_send, sock_recv, sock_close.
 */

host = "example.org"
port = 80

fd = tcp_connect(host, port)
if (fd <= 0)
  print("connect failed")
else
  req = join([
    "GET / HTTP/1.0\r\n",
    "Host: ", host, "\r\n",
    "User-Agent: fun/0.20\r\n",
    "Connection: close\r\n\r\n"
  ], "")
  sent = sock_send(fd, req)
  // Read up to 8192 bytes (first chunk)
  data = sock_recv(fd, 8192)
  print(data)
  sock_close(fd)
