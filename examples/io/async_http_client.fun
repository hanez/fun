#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-03-26
 */

/*
 * Async-ish HTTP GET using new FD helper opcodes
 *
 * Demonstrates:
 * - tcp_connect(host, port)
 * - fd_set_nonblock(fd, on)
 * - fd_poll_write(fd, timeout_ms) and fd_poll_read(fd, timeout_ms)
 * - non-blocking send/recv loop
 */

host = "example.org"
port = 80

fd = tcp_connect(host, port)
if (fd == 0)
  print("connect failed")
  exit(1)

ok = fd_set_nonblock(fd, 1)
if (ok == 0)
  print("failed to set nonblocking")
  sock_close(fd)
  exit(1)

req = "GET / HTTP/1.1\r\nHost: " + host + "\r\nConnection: close\r\n\r\n"
to_send = len(req)
sent_total = 0

// Write request in a non-blocking fashion
while (sent_total < to_send)
  // Wait until socket is writable (timeout 1000ms)
  wr = fd_poll_write(fd, 1000)
  if (wr < 0)
    print("poll write error")
    sock_close(fd)
    exit(1)
  if (wr == 0)
    // timeout, loop again
    continue

  chunk = substr(req, sent_total, to_send - sent_total)
  n = sock_send(fd, chunk)
  if (n < 0)
    print("send error")
    sock_close(fd)
    exit(1)
  sent_total = sent_total + n

// Read response non-blocking until peer closes
buf = ""
while (true)
  rd = fd_poll_read(fd, 2000) // wait up to 2s for data
  if (rd < 0)
    print("poll read error")
    break
  if (rd == 0)
    // timeout or EOF; try a final read to see if closed
    data = sock_recv(fd, 4096)
    if (len(data) == 0)
      // assume connection closed
      break
    buf = buf + data
    continue

  data = sock_recv(fd, 4096)
  if (len(data) == 0)
    // closed
    break
  buf = buf + data

sock_close(fd)

// Print the first lines of the response to show it's working
print(substr(buf, 0, 200))

/*
Expected: prints the beginning of an HTTP response from example.org
*/
