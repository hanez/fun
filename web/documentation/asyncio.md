---
layout: page
published: true
noToc: true
noComments: false
noDate: false
title: Fun - Async I/O ("asyncio") in Fun
subtitle: Async I/O primitives and patterns, non-blocking sockets, fd polling, examples, and best practices.
description: Async I/O primitives and patterns, non-blocking sockets, fd polling, examples, and best practices.
permalink: /documentation/asyncio/
lang: en
tags:
- documentation
- handbook
- installation
- usage
- introduction
- help
- guide
- howto
- docs
- specifications
- specs
- repl
---

# Async I/O ("asyncio") in Fun

This guide explains the new asynchronous I/O primitives in Fun and how to use them to build non-blocking network and file descriptor workflows. It covers the core concepts, available helpers, common patterns, and runnable examples from the repository.

## What is asyncio in Fun?

In Fun, "asyncio" refers to event-driven, non-blocking I/O built around file descriptor readiness. Instead of blocking on reads/writes, you:
- Put descriptors (sockets, pipes, etc.) into non-blocking mode
- Wait for them to become readable/writable using polling helpers
- Perform small, incremental reads/writes when the OS signals readiness

This lets a single Fun script handle many concurrent connections efficiently without threads, and keeps UIs or other work responsive while I/O is in flight.

There is no special syntax (like async/await) — you compose ordinary control flow with a few focused opcodes and stdlib functions.

However, for more ergonomic, "await-like" workflows without changing the VM, a tiny cooperative scheduler is provided in the stdlib at lib/async/scheduler.fun. It lets you write small step functions that advance per tick and use await_read/await_write wrappers for readability.

## Building blocks

Core helpers wired into the VM (see src/vm/os/*):
- fd_set_nonblock(fd, on) → 1/0: enable or disable O_NONBLOCK on a file descriptor
- fd_poll_read(fd, timeout_ms) → int: >0 if fd is readable; 0 on timeout; <0 on error
- fd_poll_write(fd, timeout_ms) → int: >0 if fd is writable; 0 on timeout; <0 on error

Common networking helpers from the stdlib:
- tcp_connect(host, port) → fd: open a TCP connection; returns 0 on failure
- sock_send(fd, data) → int: send bytes (may write only part in non-blocking mode)
- sock_recv(fd, max_bytes) → string: receive up to max_bytes; empty string on EOF
- sock_close(fd): close the descriptor

Tip: Always check return values. In non-blocking mode, partial writes and short reads are normal.

## Typical patterns

1) Connect and switch to non-blocking
```
fd = tcp_connect(host, port)
if (fd == 0)
  // handle connect error
ok = fd_set_nonblock(fd, 1)
if (ok == 0)
  // handle mode switch error
```

2) Non-blocking write loop with readiness polling
```
remaining = req
while (len(remaining) > 0)
  wr = fd_poll_write(fd, 1000)  // wait up to 1s
  if (wr < 0)
    // poll error; abort
  if (wr == 0)
    continue  // timeout; try again
  n = sock_send(fd, remaining)
  if (n < 0)
    // send error; abort
  remaining = substr(remaining, n, len(remaining) - n)
```

3) Non-blocking read-until-close
```
buf = ""
while (true)
  rd = fd_poll_read(fd, 2000)  // wait up to 2s
  if (rd < 0)
    // poll error; break
  if (rd == 0)
    // timeout: try a read to detect EOF
    data = sock_recv(fd, 4096)
    if (len(data) == 0)
      break  // likely closed
    buf = buf + data
    continue
  data = sock_recv(fd, 4096)
  if (len(data) == 0)
    break  // closed
  buf = buf + data
```

## Timeouts and responsiveness

- timeout_ms controls how long poll waits. Use small timeouts inside loops to interleave work across multiple sockets or tasks.
- A timeout result (0) is not an error — treat it as an opportunity to perform other duties and try again later.
- Negative results (<0) indicate OS-level errors from poll/select; handle or abort as appropriate.

## Working with multiple connections

To multiplex several sockets:
- Keep per-connection state (outgoing buffer, accumulate incoming, progress markers)
- Round-robin over connections, polling each for read/write readiness with short timeouts
- Advance each state machine a little per iteration

Because Fun keeps the primitives low-level and explicit, you can build simple cooperative schedulers, connection pools, or protocol handlers directly in Fun code.

## Examples in the repository

- examples/io/async_http_client.fun — Minimal HTTP GET over non-blocking TCP using fd_poll_* helpers
- examples/io/await_http_client.fun — Same goal, but written using lib/async/scheduler.fun with await_* helpers
- examples/net/http_mt_server.fun — Multi-tenant HTTP server scaffold (compare patterns for concurrency)
- examples/net/http_mt_server_cgi.fun — Server variant that dispatches CGI-like handlers
- lib/net/http_cgi_server.fun — Library helpers used by the server examples

Run client example from a build tree:
```
FUN_LIB_DIR=./lib ./build/fun examples/io/async_http_client.fun
```

If installed system-wide, just:
```
fun /usr/share/fun/examples/io/async_http_client.fun
```

Or to try the await-style client using the cooperative scheduler:
```
FUN_LIB_DIR=./lib ./build/fun examples/io/await_http_client.fun
```

## Cooperative scheduler helpers (library-level)

The file lib/async/scheduler.fun provides a minimal cooperative scheduler built on the existing primitives. There is no VM-level suspension: each task is a small state machine advanced one step per tick. API summary:

- task_spawn(step_fn, state_map) → task_handle
  - Registers a task. step_fn is a function that takes a Map state; mutate state and set state.done = 1 when complete.
- run_once() → 1
  - Performs one scheduling tick over all runnable tasks.
- run_until_done() → 1
  - Repeats run_once() with a tiny sleep_ms(1) until all tasks finish.
- await_read(fd, timeout_ms) → int
  - Wrapper over fd_poll_read; returns 1 if readable, 0 on timeout/EOF, -1 on error.
- await_write(fd, timeout_ms) → int
  - Wrapper over fd_poll_write; returns 1 if writable, 0 on timeout, -1 on error.
- yield() → 1
  - No-op helper to make intent explicit in step functions.
- async_sleep_mark(state, ms) → 1
  - Mark the task to be skipped for roughly ms milliseconds; cleared automatically when it wakes.

Example skeleton using the scheduler:
```
#include <async/scheduler.fun>

fun my_task_step(t)
  if (t.phase == nil)
    t.phase = 0

  if (t.phase == 0)
    t.fd = tcp_connect("example.org", 80)
    if (t.fd == 0)
      t.done = 1
      return
    fd_set_nonblock(t.fd, 1)
    t.phase = 1
    return

  if (t.phase == 1)
    if (await_write(t.fd, 50) == 1)
      sock_send(t.fd, "GET / HTTP/1.1\r\nHost: example.org\r\n\r\n")
      t.buf = ""
      t.phase = 2
    return

  if (t.phase == 2)
    rd = await_read(t.fd, 100)
    if (rd < 0)
      t.done = 1
      return
    if (rd == 0)
      // try a small read to detect EOF
      data = sock_recv(t.fd, 4096)
      if (len(data) == 0)
        t.done = 1
      else
        t.buf = t.buf + data
      return
    // readable
    data = sock_recv(t.fd, 4096)
    if (len(data) == 0)
      t.done = 1
    else
      t.buf = t.buf + data
    return

task = task_spawn(my_task_step, {})
run_until_done()
```

This approach is 100% compatible with current runtimes and serves as a stepping stone towards potential future VM-level async/await opcodes.

## Error handling and cleanup

- Always close descriptors with sock_close(fd) when finished or on error paths.
- Distinguish between timeout (wr/rd == 0), EOF (len(recv) == 0), and errors (wr/rd < 0 or send < 0).
- For large payloads, design your loops to tolerate partial progress and resume cleanly.

## FAQ

Q: Is there an async/await syntax?
A: Not at this time. The model is explicit non-blocking I/O with polling helpers. You can build lightweight schedulers on top if desired.

Q: Does this work on all platforms?
A: The helpers map to portable OS facilities exposed by the VM. Details may vary by platform; see documentation/troubleshooting.md and open an issue if you hit differences.

Q: How do I integrate with the REPL?
A: You can prototype small non-blocking fragments in the REPL, but full networking examples are easier to run as scripts.

## See also

- [examples.md](./examples/) — Running bundled examples
- [includes.md](./includes/) — Include paths and library discovery
- [opcodes.md](./opcodes/) — VM opcodes overview
- [troubleshooting.md](./troubleshooting/) — Common issues and quick fixes
