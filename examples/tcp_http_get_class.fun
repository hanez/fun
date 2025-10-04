#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-04
 */

#include <io/socket.fun>

// Simple HTTP GET using TcpClient class from stdlib

c = TcpClient()
if (!c.connect("example.org", 80))
  print("connect failed")
else
  req = join([
    "GET / HTTP/1.0\r\n",
    "Host: example.org\r\n",
    "User-Agent: fun/0.21\r\n",
    "Connection: close\r\n",
    "\r\n"
  ], "")
  c.send(req)
  resp = c.recv_all(8192)
  print(resp)
  c.close()
