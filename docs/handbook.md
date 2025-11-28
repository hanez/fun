---
layout: page
published: true
noToc: true
noComments: true
title: /handbook/index.fun
subtitle:
description: Handbook
permalink: /handbook/
lang: en
tags:
- handbook
- installation
- usage
- introduction
- help
- guide
- howto
---

# Fun Handbook (Second Edition)

This is a refreshed, de-duplicated, and fully up-to-date handbook for the Fun programming language and its virtual machine (VM). It keeps the same section layout as the original handbook while consolidating repeated content and documenting all currently available features, including the latest SQLite support.

## Overview

Fun is a small, strict, and simple programming language executed by a stack-based virtual machine. Most of the ecosystem is written in Fun itself; only a minimal core is implemented in C. The design focuses on simplicity, consistency, and joy in coding.

## Introduction

- Dynamic and optionally statically typed
- Type safety
- Written in C (C99) and Fun
- Minimal C core; most core functions and libraries implemented in Fun
- Internal libraries use snake_case for functions even when written in Fun; class names are CamelCase

## Installation

### Requirements

- A C compiler, a libc, and Git

FreeBSD:
- CMake
- Clang

Linux:
- CMake
- GCC (Clang should also work)

Windows:
- Cygwin (not covered in detail here)
- CMake
- GCC via Cygwin

### Build Fun

Linux/UNIX and Cygwin are covered here.

Clone repository:

```
git clone https://git.xw3.org/fun/fun.git
cd fun
```

Configure and build (examples shown with several optional features enabled):

```
# Every -D flag must be NAME=VALUE (e.g., -DFUN_WITH_REPL=ON)
cmake -S . -B build \
  -DFUN_DEBUG=OFF \
  -DFUN_WITH_REPL=ON \
  -DFUN_WITH_JSON=ON \
  -DFUN_WITH_PCRE2=ON \
  -DFUN_WITH_CURL=ON \
  -DFUN_WITH_PCSC=OFF \
  -DFUN_WITH_SQLITE=OFF
cmake --build build --target fun
```

Run the demo (without installing):

```
FUN_LIB_DIR="$(pwd)/lib" ./build/fun ./demo.fun
```

Tracing execution:

```
FUN_LIB_DIR="$(pwd)/lib" ./build/fun --trace ./demo.fun
```

Drop into the REPL when an error occurs:

```
FUN_LIB_DIR="$(pwd)/lib" ./build/fun --repl-on-error --trace ./demo.fun
```

Start the REPL directly (build with -DFUN_WITH_REPL=ON):

```
FUN_LIB_DIR="$(pwd)/lib" ./build/fun
```

#### CMake options

Pass all options as -DNAME=VALUE. The most relevant toggles are:

- FUN_DEBUG=ON|OFF — verbose VM debug logging (default OFF)
- FUN_WITH_CURL=ON|OFF — enable CURL (libcurl) support (default OFF)
- FUN_WITH_JSON=ON|OFF — enable JSON (json-c) support  (default OFF)
- FUN_WITH_LIBSQL=ON|OFF — enable libSQL (Turso) client support (default OFF)
- FUN_WITH_PCRE2=ON|OFF — enable PCRE2 (Perl-Compatible Regular Expressions) (default OFF)
- FUN_WITH_PCSC=ON|OFF — enable PC/SC smart card (PCSC lite) support (default OFF)
- FUN_WITH_REPL=ON|OFF — enable the interactive REPL (default OFF)
- FUN_WITH_SQLITE=ON|OFF — enable SQLite (sqlite3) support (default OFF)

You can also set the default search path for the bundled stdlib with DEFAULT_LIB_DIR:

```
cmake -S . -B build -DDEFAULT_LIB_DIR="/usr/share/fun/lib" -DFUN_WITH_REPL=ON
```

If you encounter a CMake error such as:

  CMake Error: Parse error in command line argument: FUN_WITH_JSON
  Should be: VAR:type=value

it means you passed a -D option without a value. Always use the form -DNAME=VALUE (e.g., -DFUN_WITH_JSON=ON).

#### SQLite example (optional feature)

SQLite support is optional and disabled by default. To build with it and run the example:

```
cmake -S . -B build -DFUN_WITH_SQLITE=ON
cmake --build build --target fun

# Create the sample database (requires the sqlite3 CLI):
sqlite3 ./database.sqlite < ./examples/data/database.sql

# Run the example
FUN_LIB_DIR="$(pwd)/lib" ./build/fun ./examples/sqlite_example.fun
```

#### libSQL example (optional feature)

libSQL support is optional and disabled by default. It is implemented as an independent extension and can coexist with SQLite. To build with it and run the example:

```
cmake -S . -B build -DFUN_WITH_LIBSQL=ON
cmake --build build --target fun

# Create the sample database using the sqlite3 CLI (libSQL implements the sqlite C API)
sqlite3 ./database.sqlite < ./examples/data/database.sql

# Run the libSQL example
FUN_LIB_DIR="$(pwd)/lib" ./build/fun ./examples/libsql_example.fun
```

Available builtins when built with -DFUN_WITH_LIBSQL=ON:
- libsql_open(path_or_url) -> handle (>0) or 0 on error
- libsql_close(handle) -> Nil
- libsql_exec(handle, sql) -> rc (0 on success)
- libsql_query(handle, sql) -> array of map rows

### Install Fun to the OS (optional)

Not recommended during early development, but supported:

```
sudo cmake --build build --target install
```

After installation, FUN_LIB_DIR usually isn’t needed because libs are placed in the system default directory (e.g., /usr/share/fun/lib).

## Usage

Run a script:

```
fun ./demo.fun
```

## Table of contents

- Language overview and VM internals
- Command line interface and REPL
- Core types and operations
- Built-ins overview (what the VM provides)
- Standard library APIs
  - io.console
  - io.process
  - io.socket
  - io.thread
  - utils.datetime
  - regex
  - crypt (MD5, SHA-1/256/384/512)
  - encoding.base64
  - arrays, strings, maps helpers (hex, range)
- Extra libraries
  - JSON (via json-c)
  - CURL (via libcurl)
  - PCSC (PC/SC smart card)
  - SQLite (sqlite3)
- Examples reference

---

## Language overview and VM internals

Fun compiles .fun source files to bytecode and executes them on a stack-based VM. Functions and methods push/pop their arguments and return values on a value stack.

High-level architecture:
- Front-end: parses .fun files, handles includes and constant folding, emits bytecode with debug markers (OP_LINE) used by tracing and REPL-on-error.
- VM core: runs a loop over opcodes (see src/bytecode.h). Values include numbers (integers), strings, arrays, maps, booleans (1/0), functions, and nil.
- Built-ins: I/O, strings, arrays, regex, date/time, OS, networking, threading, and optional JSON/PCRE2/CURL/PCSC/SQLite.

Selected VM concepts (non-exhaustive):
- Control flow: OP_JUMP, OP_JUMP_IF_FALSE, OP_RETURN
- Arithmetic/logic: OP_ADD/SUB/MUL/DIV, OP_MOD, OP_LT/LTE/GT/GTE, OP_EQ/NEQ, OP_AND/OR/NOT
- Stack helpers: OP_DUP, OP_SWAP, OP_POP
- Arrays: OP_MAKE_ARRAY, OP_INDEX_GET/SET, OP_LEN, OP_PUSH, OP_APOP, OP_INSERT/REMOVE, OP_SLICE
- Strings: OP_SUBSTR, OP_SPLIT, OP_JOIN, OP_FIND
- Maps: OP_MAKE_MAP; index ops shared with arrays
- Conversion/typing: OP_TO_NUMBER, OP_TO_STRING, OP_CAST, OP_TYPEOF, OP_UCLAMP/OP_SCLAMP
- Regex: OP_REGEX_MATCH/SEARCH/REPLACE (requires PCRE2 when built)
- Math: OP_MIN/MAX/CLAMP/ABS/POW, OP_RANDOM_SEED/RANDOM_INT
- Iteration helpers: OP_ENUMERATE, OP_ZIP
- OS/IO/network: sockets, files, processes, environment, threads, etc.
- Optional features: JSON (src/vm/json/*), CURL, PCSC, SQLite (src/vm/sqlite/*)

Error handling and debugging:
- Build with FUN_DEBUG=ON for verbose traces
- Run with --trace to print executed lines/opcodes
- Run with --repl-on-error to drop into an interactive REPL when a runtime error occurs

## Command line interface and REPL

- Run a script: fun path/to/script.fun
- Common options: --trace, --repl-on-error (can be combined). REPL requires FUN_WITH_REPL=ON at build time.
- In trace/REPL-on-error modes, the VM annotates output with file:line and function names for easier debugging (see examples/debug_reporting.fun).

## Core types and operations

Types:
- number: signed integer (with helpers for unsigned behavior)
- string: immutable bytes; len(s), join, split, substr, find
- array: ordered list; len, push, apop, insert, remove, slice
- map: associative dictionary typically keyed by strings
- boolean: represented as 1 (true) or 0 (false); operators &&, ||, !
- nil: absence of value

Control flow:
- if/else, while; range helpers in utils.range

Functions and classes:
- Define a function: fun name(args) ...
- Define a class: class Name(constructor params) with method definitions fun method(this, ...)
- _construct acts as the constructor if present; methods use explicit this

Modules and includes:
- #include <path/to/module.fun> for libs under FUN_LIB_DIR
- #include "relative/path.fun" for local includes
- Namespacing via as: #include <utils/math.fun> as m; then call m.add(...)

## Built-ins overview

Console and I/O:
- print(x) — prints value plus newline
- input(prompt) — read line from stdin

Strings and arrays:
- len(x), join(array, sep), split(text, sep), substr(text, start, len), find(text, needle)
- push(array, v), apop(array), insert(array, i, v), remove(array, i), slice(array, start, end)

Conversion and type:
- to_number(x), to_string(x), cast(value, typeName), typeof(x)
- uclamp(number, bits), sclamp(number, bits)

Math and random:
- min(a,b), max(a,b), clamp(x, lo, hi), abs(x), pow(a,b), random_seed(seed), random_int(lo, hiExclusive)

Regex (requires PCRE2 when enabled):
- regex_match(text, pattern) -> 1/0
- regex_search(text, pattern) -> map { match, start, end, groups }
- regex_replace(text, pattern, repl) -> string

OS and processes:
- proc_run(cmd) -> { out: string, code: number }
- system(cmd) -> exit code
- env_get(name), env_set(name, value)

Networking and sockets:
- tcp_connect(host, port) -> fd (>0) or 0
- sock_send(fd, data) -> bytes or -1; sock_recv(fd, maxlen) -> string; sock_close(fd)
- tcp_listen(port, backlog) -> listen fd; tcp_accept(listenFd) -> client fd
- unix_connect(path) -> fd

Threads:
- thread_spawn(func, args) -> thread id; thread_join(id) -> return value

Date and time:
- time_now_ms(), clock_mono_ms(), date_format(ms, fmt)

JSON (optional):
- json_parse(text) -> value or nil
- json_stringify(value, prettyFlag) -> string
- json_from_file(path) -> value or nil
- json_to_file(path, value, prettyFlag) -> 1/0

PC/SC (optional):
- pcsc_establish() -> context id (>0) or 0
- pcsc_list_readers(ctx) -> array of reader names or nil
- pcsc_connect(ctx, readerName) -> handle id (>0) or 0
- pcsc_disconnect(handle) -> 1/0
- pcsc_transmit(handle, bytesArray) -> { data, sw1, sw2, code }

SQLite (optional):
- sqlite_open(path) -> handle (>0) or 0 on error
- sqlite_exec(handle, sql) -> rc (0 = SQLITE_OK)
- sqlite_query(handle, sql) -> array of row maps (string keys)
- sqlite_close(handle) -> nil

Note: Optional features depend on the CMake flags used when building.

---

## Standard library APIs

The stdlib provides small wrappers around VM built-ins, typically organized in classes to avoid global name collisions and to offer sensible defaults.

### io.console

Class Console (lib/io/console.fun):
- prompt(text) -> string
- ask(question) -> string
- ask_yes_no(question) -> 1/0 (y/yes vs n/no)

Example: examples/input_example.fun

### io.process

Class Process (lib/io/process.fun):
- run(cmd) -> { out, code }
- run_merge_stderr(cmd) -> { out, code }
- system(cmd) -> number
- check_call(cmd) -> 1/0

Example: examples/process_example.fun

### io.socket

Provides TcpClient, TcpServer, UnixClient (lib/io/socket.fun).

TcpClient:
- connect(host, port) -> 1/0; is_connected() -> 1/0
- send(data) -> bytes or -1; recv(maxlen) -> string; recv_all(chunk_size) -> string
- close()

TcpServer(port, backlog):
- listen() -> listen fd or 0; accept() -> client fd
- echo_once(maxlen) -> 1 when handled; serve_forever(maxlen) -> never returns
- close()

UnixClient:
- connect(path), is_connected(), send(data), recv(maxlen), close()

Examples: tcp_http_get.fun, tcp_http_get_class.fun, unix_socket_echo.fun, extra/tcp_echo_server_class.fun

### io.thread

Class Thread (lib/io/thread.fun):
- spawn(func, args) -> thread id; join(id) -> return value
- Aliases: start(func, args), wait(id)

Examples: threads_demo.fun, thread_class_example.fun

### utils.datetime

Class DateTime (lib/utils/datetime.fun):
- now_ms(), mono_ms(), format(ms, fmt), iso_now()

Example: datetime_basic.fun

### regex

Class Regex (lib/regex.fun):
- match(text, pattern) -> 1/0
- search(text, pattern) -> { match, start, end, groups }
- replace(text, pattern, repl) -> string

Examples: regex_demo.fun, regex_procedural.fun

### crypt

MD5 (lib/crypt/md5.fun) and SHA family (sha1/sha256/sha384/sha512) provide digest classes and helpers.
Examples: md5_demo.fun, sha1_demo.fun, sha256_demo.fun, sha256_str_demo.fun, sha384_example.fun, sha512_demo.fun, sha512_str_demo.fun

### encoding.base64

Module lib/encoding/base64.fun: base64_encode(string), base64_decode(string)

### arrays, strings, maps helpers

- lib/arrays.fun — array helpers
- lib/strings.fun — string helpers (lower/upper, etc.)
- lib/hex.fun — bytes_to_hex, hex_to_bytes
- lib/utils/range.fun — numeric ranges
- lib/utils/math.fun and lib/math.fun — math helpers

---

## Extra libraries

### JSON (optional)

Build flag: -DFUN_WITH_JSON=ON; requires json-c. VM functions: json_parse, json_stringify, json_from_file, json_to_file. Stdlib class JSON wraps these with light ergonomics. Example: examples/json_showcase.fun.

### CURL (optional)

Build flag: -DFUN_WITH_CURL=ON; requires libcurl. VM provides:
- curl_get(url) -> string ("" on error)
- curl_post(url, body) -> string ("" on error)
- curl_download(url, path) -> 1/0

Examples: curl_get_json.fun, curl_post.fun, curl_download.fun

### PCSC (optional)

Build flag: -DFUN_WITH_PCSC=ON; provides pcsc_* built-ins and a stdlib wrapper class PCSC. Example: pcsc_example.fun.

### SQLite (optional)

Build flag: -DFUN_WITH_SQLITE=ON; requires sqlite3 development headers.

VM API:
- sqlite_open(path) -> handle (>0) or 0
- sqlite_exec(handle, sql) -> rc (0 = SQLITE_OK)
- sqlite_query(handle, sql) -> array of maps (columns as string keys)
- sqlite_close(handle) -> nil

Result mapping notes:
- INTEGER -> number
- FLOAT -> number (floating point)
- TEXT -> string
- NULL -> nil
- BLOB is currently not returned (mapped to nil)

Example flow (examples/sqlite_example.fun):
1) h = sqlite_open("./todo.sqlite")
2) rows = sqlite_query(h, "SELECT id, title, done, created_at FROM tasks ORDER BY id;")
3) rc = sqlite_exec(h, "INSERT INTO tasks (title, done) VALUES ('Try Fun + SQLite', 0);")
4) rows2 = sqlite_query(h, "SELECT count(*) AS cnt FROM tasks;")
5) sqlite_close(h)

---

## Examples reference

You can run examples without installing by pointing FUN_LIB_DIR to the repository lib directory:

  FUN_LIB_DIR="$(pwd)/lib" ./build/fun examples/<name>.fun

Highlights (not exhaustive):
- arrays.fun, arrays_advanced.fun, arrays_iter.fun — array operations
- booleans.fun, boolean_decl.fun — boolean basics
- builtins_conversions.fun, builtins_extended.fun — conversions and math helpers
- builtins_maps_and_more.fun — maps and indexing
- byte_for_demo.fun — bitwise operations
- class_constructor.fun, classes_demo.fun, inheritance_demo.fun — classes
- datetime_basic.fun — date/time utilities
- debug_reporting.fun, repl_on_error.fun — tracing and REPL-on-error
- exit_example.fun — exit codes
- expressions_test.fun — operators
- file_io.fun, file_print_for_file_line_by_line.fun — file I/O
- for_range_test.fun — numeric ranges
- functions_test.fun — functions and higher-order usage
- have_fun.fun — quick sanity check
- if_else_test.fun — branching
- include_lib.fun, include_local.fun, include_namespace.fun — includes and namespacing
- input_example.fun — console input
- json_showcase.fun — JSON usage
- curl_get_json.fun, curl_post.fun, curl_download.fun — HTTP via CURL
- loops_break_continue.fun, nested_loops.fun, while_test.fun — loops
- md5_demo.fun, sha1_demo.fun, sha256_demo.fun, sha256_str_demo.fun, sha384_example.fun, sha512_demo.fun, sha512_str_demo.fun — hashing
- objects_basic.fun, objects_more.fun — map/object patterns
- os_env.fun — environment variables
- pcsc_example.fun — smart card demo
- process_example.fun — running external commands
- regex_demo.fun, regex_procedural.fun — regex usage
- stdlib_showcase.fun — tour through stdlib
- strings_test.fun — string operations
- tcp_http_get.fun, tcp_http_get_class.fun — TCP client demos
- thread_class_example.fun, threads_demo.fun — threading
- try_catch_finally.fun, try_catch_with_error.fun — error handling
- typeof.fun, typeof_features.fun — types and casting
- type_safety.fun, type_safety_fails.fun — type safety
- types_integers.fun, signed_ints.fun, uint_types.fun — integers
- unix_socket_echo.fun — UNIX domain sockets
- sqlite_example.fun — SQLite usage

Notes:
- Some examples rely on optional features (JSON, CURL, PCSC, SQLite) and degrade gracefully when disabled.

---

## Internals notes (selected)

JSON: src/vm/json/* wraps json-c. OP_JSON_PARSE and friends convert json_object to Fun values and back; stdlib JSON class adds ergonomics.

PCSC: The VM interfaces with pcsc-lite/WinSCard and returns maps with data and status words. The stdlib wrapper handles absent hardware defensively.

SQLite: src/vm/sqlite/* implements open/exec/query/close using a simple handle registry. Query prepares a statement, steps rows, maps columns by name to values, and returns an array of row maps.

---

## Development

This project follows Semantic Versioning. Commit messages include the version (e.g., 1.2.3). Version bumps are made in CMakeLists.txt for code changes; documentation-only commits include the current version in the message but do not bump it.

### Development systems

- GNU/Linux (glibc, musl), FreeBSD (Clang), Windows (Cygwin + GCC). Other Unix-like systems likely work but are untested.

### Contributing and further reading

- Browse lib/ for stdlib APIs (files often document their own interfaces)
- src/bytecode.h lists supported opcodes; implementations live under src/vm/
- examples/ are the best starting point to learn by doing
