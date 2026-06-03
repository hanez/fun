---
layout: page
published: true
noToc: false
noComments: false
noDate: false
title: Redis (hiredis) Extension
subtitle: Redis client integration for Fun using the hiredis C library.
description: Documentation for the optional Redis extension in Fun. Provides redis_connect, redis_cmd, and redis_close builtins backed by hiredis.
permalink: /documentation/extensions/redis/
lang: en
tags:
- redis
- hiredis
- database
- cache
- extension
- networking
- async
---

The Redis extension adds a minimal client for Redis-compatible servers using the [hiredis](https://github.com/redis/hiredis){:class="ext"} C library. It currently exposes a simple synchronous API; Async I/O through Fun's event loop will be added later.

Requirements
------------
- Build-time option: `-DFUN_WITH_REDIS=ON`
- System libraries: `hiredis` headers and library available (via pkg-config or default linker search paths)
- Runtime: a Redis-compatible server (e.g., on `127.0.0.1:6379`)

Enabling the extension
----------------------
Example CMake configure line enabling Redis along with other options:

<pre>cmake -S . -B build_release -DCMAKE_BUILD_TYPE=Release \
  -DFUN_WITH_REDIS=ON -DFUN_WITH_OPENSSL=ON</pre>

When configured, the build summary will include a line:

<pre>Redis (FUN_WITH_REDIS): ENABLED</pre>

Provided builtins/opcodes
-------------------------
- `redis_connect(host: string, port: int) -> int`
  - Establishes a TCP connection and returns a positive handle id on success, or `0` on error.
  - Example: `h = redis_connect('127.0.0.1', 6379)`

- `redis_cmd(handle: int, cmd: string) -> Value`
  - Executes a Redis inline command string and returns the reply as a Fun value.
  - Reply mapping:
    - Status/String -> string
    - Integer -> number
    - Nil -> `nil`
    - Array -> array of recursively converted values
  - Example: `print(redis_cmd(h, 'PING'))`  ⇒ `PONG`

- `redis_close(handle: int) -> Nil`
  - Closes the connection associated with the handle and frees resources.

Notes and limitations
---------------------
- Error handling: invalid handles or failed commands yield `nil` or an empty/neutral value depending on context.
- Security: this initial version does not include TLS; TLS support may be added in a future iteration once hiredis SSL is wired in.
- Async: the current API is synchronous. Integration with Fun's asyncio is planned.

Examples
--------
Runnable examples are included in the source tree:

- `examples/extensions/redis/basic_ping.fun`
- `examples/extensions/redis/kv_set_get.fun`
- `examples/extensions/redis/list_ops.fun`
- `examples/extensions/redis/hash_ops.fun`

Quick start (from repo root, using a built Fun executable):

<pre>build_debug/fun examples/extensions/redis/basic_ping.fun</pre>

Troubleshooting
---------------
- Ensure a Redis server is reachable at the host/port you pass to `redis_connect`.
- If `redis_connect` returns `0`, verify the hiredis library and headers are installed and that Fun was configured with `-DFUN_WITH_REDIS=ON`.
