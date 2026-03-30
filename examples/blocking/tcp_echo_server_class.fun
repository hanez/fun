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

// Echo server using the stdlib TcpServer class.
// Make sure FUN_LIB_DIR points to the ./lib directory containing io/socket.fun.
// Example:
//   FUN_LIB_DIR="$(pwd)/lib" ./build/fun ./examples/tcp_echo_server_class.fun

include <io/socket.fun>

print("Starting echo server at localhost:12345")

server = TcpServer(12345, 16)

if (server.listen() <= 0)
  print("Listen failed!")
  exit(1)
else
  print("Echo server (class) listening on port 12345")

// Serve clients forever, echoing back what they send.
server.serve_forever(4096)
