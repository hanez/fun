---
layout: page
published: true
noToc: false
noComments: false
noDate: false
title: Feature Overview
subtitle: A high-level list of language, VM/runtime, tooling, and ecosystem features in Fun.
description: Comprehensive feature list of the Fun programming language, its VM/runtime, tooling, build options, and optional extensions.
permalink: /features/
lang: en
tags:
- features
- overview
- language
- vm
- runtime
- stdlib
- tooling
- build
- portability
- extensions
- web
- cgi
---

This page summarizes the core capabilities of Fun: the language, its virtual machine/runtime, tooling, build options, and the surrounding ecosystem.

---

## Language

### Syntax & Structure

- Indentation-based block structure (strict 2-space indent)
- Line comments (`//`) and block comments (`/* */`)
- Optional shebang (`#!`) line for script execution
- Unicode string support via UTF-8 throughout
- `exit` statement with optional exit code

### Type System

- **Dynamic typing** with optional **static type annotations**
- **Value types:** Integer (signed 64-bit), Float (double), Boolean, String, Array, Map, Function, Nil
- **Type annotations:** `number`, `string`, `boolean`, `float`, `nil`, `array`, `map`, `class`
- **Fixed-width integer types:** `byte` / `uint8`, `uint16`, `uint32`, `uint64`, `int8`, `int16`, `int32`, `int64` — with automatic range clamping
- **Type aliases:** `sint8`–`sint64` as synonyms for `int8`–`int64`
- `typeof()` runtime type introspection
- `to_string()` and `to_number()` conversion functions
- `cast(value, typeName)` explicit type casting

### Variables & Scope

- Global and local (per-function) variable scoping
- Auto-declaration on first assignment
- Typed variable declarations: `string name = "Fun"`
- Up to 128 globals, 64 locals per frame, 128 call frames

### Operators

- **Arithmetic:** `+`, `-`, `*`, `/`, `%` (addition also concatenates strings)
- **Comparison:** `<`, `<=`, `>`, `>=`, `==`, `!=`
- **Logical:** `&&` (and), `||` (or), `!` (not) — short-circuit evaluation
- **Bitwise (32-bit):** `band()`, `bor()`, `bxor()`, `bnot()`, `shl()`, `shr()`, `rol()`, `ror()`
- **Ternary:** `condition ? true_expr : false_expr`

### Data Structures

#### **Arrays**

- Literal syntax: `[1, 2, 3]`
- Index get/set: `arr[0]`, `arr[0] = value`
- Slice syntax: `arr[start:end]`
- Negative indices for end-relative access
- Built-in operations: `len()`, `push()`, `pop()`, `insert()`, `remove()`, `slice()`, `contains()`, `indexOf()`, `clear()`, `enumerate()`, `zip()`, `join()`, `map()`, `filter()`, `reduce()`

#### **Maps (dictionaries)**

- Literal syntax: `{"key": value, ...}`
- Bracket access/assignment: `map["key"]`, `map["key"] = value`
- Dot property access: `map.key`
- Built-in operations: `has()`, `keys()`, `values()`

### Strings

- Double and single-quoted string literals
- Concatenation with `+`
- Built-in operations: `len()`, `substr()`, `find()`, `split()`, `join()`

### Control Flow

- `if` / `else if` / `else` conditional chains
- `while` loops with `break` and `continue`
- `for var in array` — array iteration
- `for var in range(start, end)` — numeric range iteration (`[start, end)`)
- `for (key, value) in map` — map key-value iteration
- `match` expression (stdlib `lib/utils/match.fun`)

### Functions

- Named function definitions: `fun name(params) body`
- Anonymous function literals: `fn(params) body`
- First-class functions (pass as arguments, store in variables)
- Recursion support
- Return with `return expr` (or implicit nil)

### Object-Oriented Programming

- Class definitions: `class Name(typed params) body`
- Constructor method: `_construct(this, ...)` — auto-invoked on instantiation
- Methods: `fun method(this, ...)` inside class body
- Field access and mutation via `this.field`
- Property access via dot notation: `obj.field`
- Method call sugar: `obj.method(args)` (auto-binds `this`)
- Single inheritance: `class Child(...) extends Parent`
- Method overriding in subclasses

### Error Handling

- `try` / `catch` / `finally` blocks
- Error variable binding: `catch err`
- `throw` opcode for raising exceptions
- Per-frame try-stack for nested exception handlers

### Pattern Matching & Regex (Built-in POSIX)

- `regex_match(str, pattern)` — full match test (returns 1/0)
- `regex_search(str, pattern)` — first match with groups (returns map)
- `regex_replace(str, pattern, replacement)` — global search and replace

### Functional Programming

- First-class and anonymous functions
- `map(array, fn)` — transform each element
- `filter(array, fn)` — keep matching elements
- `reduce(array, init, fn)` — accumulate values
- Higher-order functions (functions that accept or return functions)
- `enumerate()` and `zip()` iteration helpers

### Concurrency

- `thread_spawn(fn, args)` — spawn a thread, returns thread ID
- `thread_join(id)` — join a thread, returns its result
- Cooperative async scheduler (stdlib `lib/async/scheduler.fun`)

---

## Virtual Machine & Runtime

### Architecture

- Compact stack-based bytecode VM written in **C99**
- Tagged union value type supporting 8 runtime types
- ~220 opcodes covering all language features
- Separate operand stack (1024 entries), call frames (128 max), and globals (128)
- Each frame has 64 local slots and a 16-entry try/catch stack

### Memory & Performance

- Deterministic execution model
- Reference-counted arrays and maps
- Function/data sectioning with linker GC for small binaries
- LTO (Link-Time Optimization) support for Release builds

### Debugging & Tracing

- Built-in debugger with breakpoints (up to 64)
- Step, next, finish, and continue commands
- `--trace` / `-t` flag for opcode-level execution tracing
- Per-opcode execution counters (compile-time `FUN_TRACE`)
- `--repl-on-error` flag: drops into interactive REPL on runtime error with stack preserved
- Stack trace printing on errors
- Source line mapping in error messages (includes include-file resolution)

### I/O & Platform

- `print()` — output with newline
- `echo()` — output without newline (immediate flush)
- `read_file(path)` — read entire file into string
- `write_file(path, data)` — write string to file
- `input_line()` — read a line from stdin (with optional prompt)
- `env(name)` / `env_all()` — get environment variables
- `proc_run(cmd)` — run command, capture stdout+exit code
- `system(cmd)` — run command via shell, returns exit code
- `os_list_dir(path)` — list directory entries

### Date, Time & Random

- `time_now_ms()` — wall clock in milliseconds since Unix epoch
- `clock_mono_ms()` — monotonic clock for interval measurement
- `date_format(ms, fmt)` — format timestamps via strftime
- `sleep(ms)` — suspend execution
- `random_seed(seed)` — seed the PRNG
- `random_int(lo, hi)` — random integer in `[lo, hi)`
- `random_number(len)` — cryptographically random hex string

### Networking (Built-in, Unix)

- `sock_tcp_listen(port, backlog)` — TCP server socket
- `sock_tcp_accept(listen_fd)` — accept client connection
- `sock_tcp_connect(host, port)` — TCP client connection
- `sock_send(fd, data)` / `sock_recv(fd, maxlen)` — send/receive data
- `sock_unix_listen(path, backlog)` / `sock_unix_connect(path)` — Unix domain sockets
- `sock_close(fd)` — close socket
- `fd_set_nonblock(fd, on)` — non-blocking mode
- `fd_poll_read(fd, timeout_ms)` / `fd_poll_write(fd, timeout_ms)` — I/O readiness polling

### Serial Communication (Unix)

- `serial_open(path, baud_rate)` — open serial port
- `serial_config(fd, data_bits, parity, stop_bits, flow_control)` — configure
- `serial_send(fd, data)` / `serial_recv(fd, maxlen)` — send/receive
- `serial_close(fd)` — close

### Integer Utilities

- `sclamp(value, bits)` / `uclamp(value, bits)` — signed/unsigned bit-width clamping
- Integer type declarations (`byte`, `uint8`–`uint64`, `int8`–`int64`) with automatic range clamping
- `gcd(a, b)`, `lcm(a, b)` — greatest common divisor, least common multiple
- `isqrt(x)` — integer square root
- `sign(x)` — signum (-1, 0, 1)

---

## Standard Library

The standard library is written primarily in **Fun itself** and lives in `lib/`:

### Strings (`lib/strings.fun`)

- `str_ltrim`, `str_rtrim`, `str_trim` — whitespace trimming
- `str_starts_with`, `str_ends_with` — prefix/suffix checking
- `str_split` — single-character delimiter splitting
- `str_replace_all` — global substring replacement
- `str_to_lower`, `str_to_upper` — ASCII case conversion
- `str_repeat` — string repetition
- `string_to_bytes_ascii` — ASCII string to byte array

### Arrays (`lib/arrays.fun`)

- `array_slice`, `array_reverse`, `array_concat` — slicing and combining
- `array_index_of`, `array_contains` — searching
- `array_unique` — deduplication
- `array_flatten1` — flatten one level of nesting

### Math (`lib/math.fun`)

- `abs`, `clamp`, `gcd`, `lcm`, `powi` (integer exponentiation)
- `min3`, `max3`, `array_min`, `array_max`

### Hex (`lib/hex.fun`)

- `hex_to_dec`, `dec_to_hex`, `hex_to_bytes`, `bytes_to_hex`

### Encoding (`lib/encoding/base64.fun`)

- `b64_encode_bytes`, `b64_decode_to_bytes` — Base64 encoding/decoding

### Cryptography (Pure Fun implementations in `lib/crypt/`)

- **MD5** — `MD5` class (`lib/crypt/md5.fun`)
- **SHA-1** — `SHA1` class (`lib/crypt/sha1.fun`)
- **SHA-256** — `SHA256` class (`lib/crypt/sha256.fun`)
- **SHA-384** — `SHA384` class (`lib/crypt/sha384.fun`)
- **SHA-512** — `SHA512` class (`lib/crypt/sha512.fun`)
- **CRC-32** — `CRC32` class (IEEE 802.3)
- **CRC-32C** — `CRC32C` class (Castagnoli)
- **AES-256** — `AES256` class (ECB mode)

### Functional Utilities

- **Option type** (`lib/utils/option.fun`): `some()`, `none()`, `is_some`, `is_none`, `unwrap`, `unwrap_or`, `option_map`, `and_then`, `or_else`, `try_get`
- **Result type** (`lib/utils/result.fun`): `ok()`, `err()`, `is_ok`, `is_err`, `unwrap`, `unwrap_or`, `result_map`, `map_err`, `and_then`, `or_else`, `to_option`
- **Pattern matching** (`lib/utils/match.fun`): `match(value, cases)` with `is`, `when`, `else` patterns

### Range Utilities (`lib/utils/range.fun`)

- `range(n)` — `[0, n)`
- `range2(start, end)` — `[start, end)`
- `range3(start, end, step)` — stepped range

### Date/Time (`lib/utils/datetime.fun`)

- `DateTime` class with `now_ms`, `mono_ms`, `format`, `iso_now`, `iso_from`, `date_str`, `time_str`, `today_str`, `start_timer`, `elapsed_ms`, `sleep_ms`, `sleep_s`

### CLI (`lib/cli.fun`)

- `argv()` — retrieve command-line arguments
- `parse_args(args)` — parse flags and positional arguments

### Console (`lib/io/console.fun`)

- `Console` class with `prompt`, `ask`, `ask_hidden`, `ask_yes_no`, `term_cols`, `progress` (progress bar)

### Process (`lib/io/process.fun`)

- `Process` class wrapping `proc_run` and `system`

### Thread (`lib/io/thread.fun`)

- `Thread` class wrapping `thread_spawn` / `thread_join`

### Socket Classes (`lib/io/socket.fun`)

- `TcpClient` — TCP client with connect/send/recv/close/recv_all
- `TcpServer` — TCP server with listen/accept/close
- `UnixClient` — Unix domain socket client

### Serial (`lib/io/serial.fun`)

- `Serial` class wrapping serial port operations

### Async Scheduler (`lib/async/scheduler.fun`)

- Cooperative multitasking with `task_spawn`, `co_yield`, `run_once`, `run_until_done`
- I/O readiness polling: `await_read`, `await_write`

### Networking / Web

- **CGI** (`lib/net/cgi.fun`): full CGI request parsing, response generation, URL encoding/decoding
- **HTTP Server** (`lib/net/http_server.fun`): static file serving with `.fun` script execution
- **HTTP CGI Server** (`lib/net/http_cgi_server.fun`): CGI-based HTTP server
- **IRC Client** (`lib/net/irc.fun`): IRC protocol client with message parsing

---

## Optional Extensions (Build-time)

Enabled via CMake flags; each wraps a mature C library:

| Extension | CMake Flag | Library | Features |
|-----------|-----------|---------|----------|
| **[JSON](/documentation/extensions/json/)** | `FUN_WITH_JSON` | [json-c](https://json-c.github.io/json-c/){:class="ext"} | `json_parse()`, `json_stringify()`, `json_from_file()`, `json_to_file()` |
| **[cURL](/documentation/extensions/curl/)** | `FUN_WITH_CURL` | [libcurl](https://curl.se/libcurl/){:class="ext"} | `curl_get()`, `curl_post()`, `curl_download()` |
| **[SQLite](/documentation/extensions/sqlite/)** | `FUN_WITH_SQLITE` | [libsqlite3](https://www.sqlite.org/){:class="ext"} | `sqlite_open()`, `sqlite_close()`, `sqlite_exec()`, `sqlite_query()` |
| **[PCRE2](/documentation/extensions/pcre2/)** | `FUN_WITH_PCRE2` | [libpcre2](https://www.pcre.org/){:class="ext"} | `pcre2_test()`, `pcre2_match()`, `pcre2_find_all()` — with flags (i, m, s, u, x) |
| **[OpenSSL](/documentation/extensions/openssl/)** | `FUN_WITH_OPENSSL` | [libcrypto](https://www.openssl.org/){:class="ext"} | `openssl_md5()`, `openssl_sha256()`, `openssl_sha512()`, `openssl_ripemd160()` |
| **[INI](/documentation/extensions/ini/)** | `FUN_WITH_INI` | [iniparser](https://github.com/ndevilla/iniparser){:class="ext"} | `ini_load()`, `ini_get_string/int/double/bool()`, `ini_set()`, `ini_unset()`, `ini_save()` |
| **[XML](/documentation/extensions/xml2/)** | `FUN_WITH_XML2` | [libxml2](http://xmlsoft.org/){:class="ext"} | `xml_parse()`, `xml_root()`, `xml_name()`, `xml_text()` |
| **[PC/SC](/documentation/extensions/pcsc/)** | `FUN_WITH_PCSC` | [libpcsclite](https://pcsclite.apdu.fr/){:class="ext"} | `pcsc_establish()`, `pcsc_list_readers()`, `pcsc_connect()`, `pcsc_transmit()`, etc. |
| **[KCGI](/documentation/extensions/kcgi/)** | `FUN_WITH_KCGI` | [libkcgi](https://kristaps.bsd.lv/kcgi/){:class="ext"} | `kcgi_parse()`, `kcgi_reply_start()`, `kcgi_write()`, `kcgi_end()` |
| **[Redis/Valkey](/documentation/extensions/redis/)** | `FUN_WITH_REDIS` | [hiredis](https://github.com/redis/hiredis){:class="ext"} | `redis_connect()`, `redis_cmd()`, `redis_close()` |

Some extensions also have a corresponding **stdlib wrapper class** in `lib/io/` or `lib/net/`:

- `JSON` class (`lib/io/json.fun`)
- `INI` class (`lib/io/ini.fun`)
- `XML` class (`lib/io/xml.fun`)
- `PCSC` / `PCSC2` classes (`lib/io/pcsc.fun`)
- `PCRE2` class (`lib/regex/pcre2.fun`)
- `KCGI` class (`lib/net/kcgi.fun`)

---

## FFI / Interop (Experimental)

### Rust FFI (`FUN_WITH_RUST`)

- Cargo-based Rust static library linked into the VM
- Demo opcodes: `rust_hello()`, `rust_hello_args()`, `rust_hello_args_return()`, `rust_get_sp()`, `rust_set_exit()`
- Rust has unsafe access to VM internals via raw pointer and struct offset APIs

### C++ FFI (`FUN_WITH_CPP`)

- C++ static library linked into the VM
- Demo opcode: `cpp_add(a, b)`

---

## Tooling

- **`fun`** — Interpreter/REPL. Runs `.fun` scripts or starts interactive REPL
- **`funstx`** — Syntax checker with optional `--fix` mode
- **REPL** — Interactive shell with history (1000 lines), multi-line input, tab completion, and commands: `:help`, `:env`, `:load`, `:run`, `:edit`, `:save`, `:clear`, `:exit`, `:quit`, `:debug`, `:import`, `:export`, `:type`, `:trace`, `:reload`
- **Test harnesses** — `fun_test` (bytecode-level tests) and `test_opcodes` (opcode exercisers)
- **CTest integration** — crypto example scripts run as automated tests
- **clang-format** target for consistent C source formatting

---

## Build System

- **CMake** 3.10+ with C99 standard
- Build toggles: `FUN_DEBUG`, `FUN_USE_MUSL`, `FUN_WITH_REPL`, `FUN_WITH_CPP`, `FUN_WITH_RUST`, `FUN_BUILD_DOCS`
- Per-extension toggles for all optional libraries
- Release builds with LTO, function/data sectioning, and `--gc-sections`
- Doxygen documentation generation
- Install targets for binaries, libraries, examples, man pages
- Uninstall target with safe directory cleanup

---

## Portability

- Written in **C99** with POSIX extensions
- Primary target: **Linux** (glibc and musl)
- Partial **Windows** support
- Platform abstraction for threading, sockets, serial I/O
- Builds with GCC and Clang

---

## Testing

- Unit tests for VM opcodes and bytecode execution
- CTest integration runs example scripts as automated tests
- Example scripts in `examples/` (90+ files) cover all language features
- Crypto self-tests for MD5, SHA-1/256/384/512, CRC-32/CRC-32C, AES-256
- Include-line mapping regression test
- KCGI smoke test (when enabled)

---

## Documentation

- Jekyll-based website under `web/` with full documentation
- Doxygen API reference
- Language specification documents (`spec/`)
- Over 90 annotated example scripts
- Handbook, type system guide, REPL guide, testing guide
- Opcode reference with stack effect documentation
- Changelog and semantic versioning

---

## Licensing

- **Apache 2.0** — fully open source, freely usable and modifiable

