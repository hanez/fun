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

fd = sock_connect("127.0.0.1", 9090)
if (fd <= 0)
  print("connect failed")
  return 0
sock_send(fd, "ping\n")
resp = sock_recv(fd, 4096)
print("response: " + resp)
sock_close(fd)

/* Expected output (with tcp_echo_server.fun running):
response: ping
*/
