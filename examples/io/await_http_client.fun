#!/usr/bin/env fun

/*
 * Async-style HTTP GET using lib/async/scheduler.fun helpers
 *
 * This demonstrates building an "await-like" workflow without VM-level
 * async/await. We use a cooperative task that advances a small state
 * machine each tick, calling await_write/await_read to probe readiness.
 */

#include <async/scheduler.fun>

host = "example.org"
port = 80

// Task state and step function
fun http_get_step(t)
  // phases: 0=connect, 1=set nonblock, 2=send, 3=recv, 4=done
  if (t.phase == nil)
    t.phase = 0

  if (t.phase == 0)
    t.fd = tcp_connect(host, port)
    if (t.fd == 0)
      print("connect failed")
      t.done = 1
      return
    t.phase = 1
    return

  if (t.phase == 1)
    ok = fd_set_nonblock(t.fd, 1)
    if (ok == 0)
      print("failed to set nonblocking")
      sock_close(t.fd)
      t.done = 1
      return
    t.req = "GET / HTTP/1.1\r\nHost: " + host + "\r\nConnection: close\r\n\r\n"
    t.sent = 0
    t.phase = 2
    return

  if (t.phase == 2)
    // send request in chunks when writable
    if (t.sent < len(t.req))
      // short timeout to keep loop responsive
      wr = await_write(t.fd, 50)
      if (wr < 0)
        print("poll write error")
        sock_close(t.fd)
        t.done = 1
        return
      if (wr == 0)
        // not yet writable; yield to others
        return
      chunk = substr(t.req, t.sent, len(t.req) - t.sent)
      n = sock_send(t.fd, chunk)
      if (n < 0)
        print("send error")
        sock_close(t.fd)
        t.done = 1
        return
      t.sent = t.sent + n
      return
    // all sent; switch to receive
    t.buf = ""
    t.phase = 3
    return

  if (t.phase == 3)
    rd = await_read(t.fd, 100)
    if (rd < 0)
      print("poll read error")
      sock_close(t.fd)
      t.phase = 4
      t.done = 1
      return
    if (rd == 0)
      // try a small read to detect close, else wait more
      data = sock_recv(t.fd, 4096)
      if (len(data) == 0)
        sock_close(t.fd)
        t.phase = 4
        t.done = 1
        return
      t.buf = t.buf + data
      return
    // readable
    data = sock_recv(t.fd, 4096)
    if (len(data) == 0)
      sock_close(t.fd)
      t.phase = 4
      t.done = 1
      return
    t.buf = t.buf + data
    return

  if (t.phase == 4)
    // print first 200 chars and finish
    print(substr(t.buf, 0, 200))
    t.done = 1
    return


// Spawn task and run
task = task_spawn(http_get_step, {})
run_until_done()

/*
Expected: prints the beginning of an HTTP response from example.org
*/
