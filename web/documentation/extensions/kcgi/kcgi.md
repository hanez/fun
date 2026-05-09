layout: page
published: true
noToc: false
noComments: false
noDate: false
title: Fun - KCGI extension (optional)
subtitle: Documentation for KCGI (kcgi) extension (optional)
description: Documentation for KCGI (kcgi) extension (optional)
permalink: /documentation/extensions/kcgi/
lang: en
tags:
- extension
- kcgi
- optional
---

- CMake option: FUN_WITH_KCGI=ON
- Purpose: integrate the kcgi (CGI/FastCGI) C library; provide request parsing and response helpers for building CGI apps in Fun.
- Homepage: [https://kristaps.bsd.lv/kcgi/](https://kristaps.bsd.lv/kcgi/){:class="ext"}

## Build notes:

- Requires system kcgi development headers and libraries (pkg-config name: kcgi).
- If pkg-config is not available, build falls back to linking against libkcgi and zlib (as configured in cmake/Extensions/KCGI.cmake).

## Provided helper/opcodes:

- Function: kcgi_parse(data: none) -> Map | Nil. Parses the current CGI/FastCGI request via kcgi and returns a Map with keys like method, scheme, host, port, path, suffix, query, and fields (GET/POST map). Returns Nil on failure or when the extension is disabled.
- Function: kcgi_reply_start(code:int, content_type:string) -> 1/0. Starts the HTTP reply (sets Content-Type and opens the body).
- Function: kcgi_write(text:string) -> 1/0. Writes a chunk to the response body.
- Function: kcgi_end() -> 1/0. Finalizes the response and frees request resources.
- Opcodes: OP_KCGI_PARSE, OP_KCGI_REPLY_START, OP_KCGI_WRITE, OP_KCGI_END (internal mappings for the functions above).

## Quickstart:

- Configure: cmake -S . -B build -DFUN_WITH_KCGI=ON
- Build: cmake --build build --target fun
- Run example:
  - FUN_LIB_DIR="$(pwd)/lib" ./build/fun examples/cgi/hello_kcgi.fun

## Example output:

- Content-Type: text/html; charset=utf-8
- <h1>Hello, Fun!</h1>

## Notes:

- Running outside a real CGI/FastCGI environment may emit RFC warnings (e.g., missing REMOTE_ADDR); these are benign for local testing.
- When FUN_WITH_KCGI is OFF, the helpers behave as no-ops (returning Nil/0) to keep scripts portable.
