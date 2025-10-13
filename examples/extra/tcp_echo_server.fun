#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-13
 */

// Simple TCP echo server using built-in socket ops.
// Listens on localhost:12345 and echoes a single line/request back to each client.

port = 12345
backlog = 16

listen_fd = tcp_listen(port, backlog)
if (listen_fd <= 0)
  print("tcp_listen failed")
  exit(1)

print("Echo server listening on port " + to_string(port))

while (true)
  client = tcp_accept(listen_fd)
  if (client > 0)
    data = sock_recv(client, 4096)
    if (len(data) > 0)
      // Echo back exactly what we received
      sock_send(client, data)
    sock_close(client)

// Not reached in this example, but here for completeness
sock_close(listen_fd)
