---
layout: page
published: true
noToc: false
noComments: false
noDate: false
title: Fun - HTTP server examples: architecture and how they work
subtitle: Documentation for HTTP server examples, architecture and how they work
description: Documentation for HTTP server examples, architecture and how they work
permalink: /documentation/examples/net/httpserver/
lang: en
tags:
- architecture
- example
- examples
- http
- network
- server
- they
- work
---


This guide explains the HTTP server examples under `./examples/net/http_*` and the supporting standard library modules under `./lib/net/`.

It covers what each example does, how to run it, and how the request handling/CGI pieces are implemented.

## Prerequisites
- Build the project so you have a `fun` executable, or ensure `fun` is on your PATH.
  - Typical local builds: `./build_debug/fun` or `./build_release/fun`.
- For examples that serve files/CGI scripts, content is read from `./examples/data/htdocs` by default.
- Helpful env vars used by some examples:
  - `FUN_LIB_DIR` — path to the stdlib (`./lib` when running from the repo).
  - `FUN_EXEC` — override path to the `fun` interpreter used to run CGI children.
  - `FUN_HTDOCS` — override htdocs directory for some examples (not all).
  - `FUN_PORT` — override port for some examples (mainly the MT CGI one).

## Supporting stdlib modules (lib/net)

- `net/http_server.fun` — Simple blocking HTTP server class serving static files, with a very small built-in “CGI for .fun files” (spawns `fun` with `QUERY_STRING`/`POST_DATA`).
- `net/http_cgi_server.fun` — Blocking server with fuller CGI support using `net/cgi.fun` to translate CGI output into proper HTTP responses and richer request/header parsing.
- `net/http_cgi_lib_server.fun` — Variant of the CGI server with the same core behavior, explicitly wiring the Fun interpreter path and using `CGI().cgi_to_http_response()` from `net/cgi.fun`.
- `net/cgi.fun` — Helpers to work with CGI-style programs and to translate CGI output (headers + body) into full HTTP/1.1 responses.

See documentation/stdlib.md → net/ for a quick index of these modules.

## Example: http_static_server.fun
- File: `examples/net/http_static_server.fun`
- Purpose: Minimal, hand-written HTTP server that ignores the request path and always responds with a fixed HTML page.
- Key includes: `io/socket.fun`
- Port/backlog: hard-coded `8088`, backlog 10.
- Flow:
  - Creates `TcpServer(port, backlog)` → `listen()` → loop on `accept()`
  - `sock_recv()` reads and discards request
  - Constructs a literal HTTP/1.1 200 response with `Content-Length` and `Connection: close`
  - Sends the response with `sock_send()` and closes the client
- Run:
  - `./examples/net/http_static_server.fun`
  - Open http://127.0.0.1:8080/

## Example: http_server.fun (static + simple .fun CGI)
- File: `examples/net/http_server.fun`
- Uses: `#include <net/http_server.fun>` (class `HTTPServer`)
- Purpose: Blocking server that serves static files from an htdocs directory; if the requested path ends with `.fun`, it spawns `fun` to run the script and returns its output.
- Defaults in example:
  - Port: `8080`
  - Htdocs: `./examples/data/htdocs`
- Implementation highlights (see `lib/net/http_server.fun`):
  - Parses request line to get `method` and `path`; `/` maps to `/index.html`.
  - When path ends with `.fun`, builds a small environment:
    - `QUERY_STRING` (for `?a=b`), `POST_DATA` (raw body when `POST`).
    - Runs `proc_run("<env> fun <script>")` and uses `out` as response body.
  - Otherwise reads the file from `htdocs` and serves it as-is.
  - Sends basic HTTP/1.1 headers: 200/404, `Content-Type: text/html`, `Content-Length`, `Connection: close`.
- Run:
  - `./examples/net/http_server.fun`
  - Open http://127.0.0.1:8080/ and the sample CGI endpoints like `/hello.fun?name=Fun`.

## Example: http_server_cgi.fun (CGI via external interpreter)
- File: `examples/net/http_server_cgi.fun`
- Uses: `#include <net/http_cgi_server.fun>` (class `HTTPCGIServer`)
- Purpose: Blocking server that serves static files and executes `.fun` scripts under htdocs via a child Fun interpreter, with fuller CGI environment and header handling.
- Defaults in example:
  - Port: `8080`
  - Htdocs: `./examples/data/htdocs`
- Implementation highlights (see `lib/net/http_cgi_server.fun`):
  - Robust request-line extraction and header parsing (case-normalized to uppercase).
  - Path routing: `/` → `/index.html`; `.fun` → treat as CGI script.
  - Builds CGI env including: `FUN_LIB_DIR`, `REQUEST_METHOD`, `QUERY_STRING`, `SCRIPT_NAME`, `PATH_INFO`, `SERVER_NAME`, `SERVER_PORT`, `SERVER_PROTOCOL`, `HTTP_HOST`, `HTTP_USER_AGENT`, `HTTP_COOKIE`, `CONTENT_TYPE`, `CONTENT_LENGTH`, and `POST_DATA` (if any).
  - Interpreter selection priority: `$FUN_EXEC` → `./build_debug/fun` → `./build_release/fun` → `fun` from PATH.
  - Uses `net/cgi.fun` to convert CGI output (headers + body) into a proper HTTP/1.1 response before sending.
- Run:
  - `./examples/net/http_server_cgi.fun`
  - Try: `/`, `/hello.fun?name=Fun`, `/info.fun` under http://127.0.0.1:8080/

## Example: http_server_cgi_lib.fun (CGI via stdlib wrapper)
- File: `examples/net/http_server_cgi_lib.fun`
- Uses: `#include <net/http_cgi_lib_server.fun>` (class `HTTPCGILibServer`)
- Purpose: Same goal as the previous example but split into a slightly different stdlib class; also uses `CGI().cgi_to_http_response()` for translating CGI output.
- Defaults and behavior mirror `HTTPCGIServer`: static files from htdocs, `.fun` as CGI with the same env block and interpreter selection logic.
- Run:
  - `./examples/net/http_server_cgi_lib.fun`
  - Try: `/`, `/hello.fun?name=Fun`, `/info.fun` under http://127.0.0.1:8080/

## Example: http_mt_server.fun (thread-per-connection)
- File: `examples/net/http_mt_server.fun`
- Uses: `io/socket.fun`, `io/thread.fun`
- Purpose: Hand-written multi-threaded server; main thread blocks in `accept()`, each connection handled in a new Fun thread.
- Behavior: Reads request, replies with a fixed HTML 200 response, then closes the connection.
- Defaults: `PORT = 8080`, `BACKLOG = 128` (note the comment mentions 8089 in a usage line; the code uses 8080).
- Run: `./examples/net/http_mt_server.fun` then open http://127.0.0.1:8080/

## Example: http_mt_server_cgi.fun (thread-per-connection + CGI)
- File: `examples/net/http_mt_server_cgi.fun`
- Uses: `io/socket.fun`, `io/thread.fun`, `net/cgi.fun`
- Purpose: Multi-threaded server that serves static files from htdocs and executes `.fun` scripts as CGI, implemented without higher-level string helpers to keep per-thread globals minimal.
- Defaults: `PORT = 8080`, `BACKLOG = 128`, `HTDOCS = ./examples/data/htdocs`.
- Request handling flow:
  - Reads request bytes, extracts the request line, splits out `method` and `target`.
  - Splits `path` and `query` on `?`; `/` becomes `/index.html`.
  - Parses headers into a map (uppercase keys) and optionally reads a request body.
  - Routing:
    - If `path` ends with `.fun`: build CGI env; spawn a child `fun` to execute the script; capture stdout; translate to HTTP via `_cgi_to_http_response()`; send back.
    - Else: attempt to read the static file from `HTDOCS + path`; send 200 or 404.
- CGI environment variables set (subset):
  - `FUN_LIB_DIR`, `REQUEST_METHOD`, `QUERY_STRING`, `SCRIPT_NAME`, `PATH_INFO`, `SERVER_NAME`, `SERVER_PORT`, `SERVER_PROTOCOL`, `HTTP_HOST`, `HTTP_USER_AGENT`, `HTTP_COOKIE`, `CONTENT_TYPE`, `CONTENT_LENGTH`, `POST_DATA` (when present).
- Interpreter selection (similar to CGI examples): `$FUN_EXEC` → `./build_debug/fun` → `./build_release/fun` → `fun` from PATH.
- Notes:
  - Allows overriding `HTDOCS` & port via `FUN_HTDOCS` and `FUN_PORT` environment variables.
  - Implements its own small helpers (`_trim`, `_ends_with`, `_split_*`) to avoid heavy string utilities inside threads.
- Run: `./examples/net/http_mt_server_cgi.fun` then open http://127.0.0.1:8080/

## Example: http_server_test.fun
- File: `examples/net/http_server_test.fun`
- Purpose: Small test harness to exercise/verify server pieces (implementation details may change). Check the source for exact behavior.

## Common behaviors and notes
- Index handling: most servers map `/` to `/index.html` under the configured `htdocs` directory.
- Static files: served by reading from `<htdocs><path>`; a missing file returns `404 Not Found`.
- CGI scripts:
  - Any path ending in `.fun` under `htdocs` is executed with the Fun interpreter as a child process.
  - CGI output is expected to be a mix of optional headers and a body; the servers convert this to a valid HTTP/1.1 response (via `net/cgi.fun` or a local helper).
  - To ensure the child can `#include` stdlib modules, `FUN_LIB_DIR` is populated (defaults to `./lib` when run from repo root).
- Content types: examples return `Content-Type: text/html; charset=utf-8` by default for dynamic responses; static file content types are not auto-detected in these examples.
- Connection handling: responses include `Connection: close`; examples do not implement keep-alive or HTTP/1.1 request pipelining.
- Security: these are demo servers. Do not expose them to untrusted networks. They lack path normalization, MIME sniffing, directory traversal protection, rate limiting, and TLS.

## How to point htdocs somewhere else
- For class-based servers: call `set_htdocs("/path/to/site")` on the server instance before `start()`.
- For the MT CGI example: set `FUN_HTDOCS=/path/to/site` in the environment before launch.

## Troubleshooting
- If a CGI request yields a 500 and you see “CGI produced no output”, run the target `.fun` directly with your `fun` interpreter and fix any errors.
- Confirm `FUN_LIB_DIR` points to the stdlib (especially when running CGI scripts that `#include` modules).
- If the server cannot start, another process might be using the chosen port.
