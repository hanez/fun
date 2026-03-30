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

port = 9090
srv = TcpServer(port, 10)
if (srv.listen() <= 0)
  print("listen failed on :" + to_string(port))
  return 0
print("Echo server on :" + to_string(port))
while true
  fd = srv.accept()
  if (fd > 0)
    msg = sock_recv(fd, 4096)
    if (len(msg) > 0) sock_send(fd, msg)
    sock_close(fd)

/* Expected output (on start):
Echo server on :9090

Then, from another shell: `nc 127.0.0.1 9090` and type "ping" — the server echoes it back.
*/
