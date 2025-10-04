#!/usr/bin/env fun

/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-04
 */

/*
 * UNIX domain socket echo demo.
 * - On Unix-like systems: demonstrates unix_listen + tcp_accept + unix_connect + sock_send/recv.
 * - On Windows: prints a friendly message and exits (since AF_UNIX is not available here).
 */

#include <io/socket.fun>

fun server_thread(listen_fd)
  // Accept a single client, read message, echo back, close.
  cfd = tcp_accept(to_number(listen_fd))
  if (cfd <= 0)
    // accept failed
    return 0
  msg = sock_recv(cfd, 4096)
  // Simple echo prefix
  _ = sock_send(cfd, join(["echo:", msg], ""))
  _ = sock_close(cfd)
  return 1

// Cross-platform guard: Windows sets OS=Windows_NT
os = env("OS")
if (os == "Windows_NT")
  print("UNIX domain sockets are not supported on Windows in this build. Skipping demo.")
else
  // Path for the UNIX socket
  path = "/tmp/fun_unix_echo.sock"

  // Start a UNIX listening socket
  lfd = unix_listen(path, 1)
  if (lfd <= 0)
    print(join(["unix_listen failed at ", path], ""))
  else
    // Spawn the server thread which will accept exactly one client
    tid = thread_spawn(server_thread, lfd)

    // Small delay to ensure the server is accepting
    sleep(50)

    // Create a client using the UnixClient from stdlib
    uc = UnixClient()
    if (!uc.connect(path))
      print("unix_connect failed")
    else
      // Send a message and read the reply
      _ = uc.send("hello over AF_UNIX")
      reply = uc.recv(4096)
      print(reply)
      _ = uc.close()

    // Join the server thread and close the listening socket
    _ = thread_join(tid)
    _ = sock_close(lfd)

/* Expected output:
echo:hello over AF_UNIX
*/
