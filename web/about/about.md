---
layout: page
title: About
subtitle: More information about having fun... :)
description: About
_date: 2023-09-20
metasub: news
noToc: false
noDate: false
tags:
- fun
- programming
- language
- about
_description: The programming language that just makes fun!
noComments: true
permalink: /about/
---

## What is Fun?

Fun is a small, strict, and simple programming language that runs on a compact stack-based virtual machine. The C core is intentionally minimal; most functionality and standard libraries are implemented in Fun itself.

Fun is dynamically typed with optional static type annotations, <a href="/features/">featuring first-class functions</a>, classes with inheritance, pattern matching, and a rich standard library. It supports everything from basic scripting to TCP sockets, serial communication, threading, cryptography (MD5, SHA-1/256/384/512, CRC-32, AES-256), and a built-in debugger.

Influenced by: **[Bash](https://www.gnu.org/software/bash/){:class="ext"}**, **[C](https://en.wikipedia.org/wiki/The_C_Programming_Language){:class="ext"}**, **[Lua](https://www.lua.org/){:class="ext"}**, **[PHP](https://www.php.net/){:class="ext"}**, **[Python](https://www.python.org/){:class="ext"}**, and **[Rust](https://www.rust-lang.org/){:class="ext"}**.

Fun is an experiment - just for fun - but Fun works!

Fun is and will ever be 100% free under the terms of the [Apache-2.0 License](https://opensource.org/license/apache-2-0){:class="ext"}.

---

## Idea

- Simplicity
- Consistency
- Simple to extend
- Hackable
- Joy in coding
- Fun!

## Characteristics

- **Dynamic typing** with optional **static type annotations** (`number`, `string`, `boolean`, `float`, `byte`, `uint8`&ndash;`uint64`, `int8`&ndash;`int64`)
- **Stack-based bytecode VM** written in C99 with ~220 opcodes
- **First-class functions**, anonymous functions (`fn`), and higher-order operations (`map`, `filter`, `reduce`)
- **Classes** with constructors, methods, and single inheritance
- **Exception handling** with `try`/`catch`/`finally`
- **Built-in data structures**: arrays (with slicing), maps/objects, strings
- **Concurrency**: threads (`thread_spawn`, `thread_join`) and cooperative async scheduler
- **Networking**: TCP and Unix domain sockets with non-blocking I/O polling
- **Serial communication**: full termios-based serial port control
- **Cryptography**: pure-Fun implementations of MD5, SHA-1/256/384/512, CRC-32/CRC-32C, AES-256 ECB
- **Built-in debugger** with breakpoints, step/next/finish, and stack inspection
- **Minimal C core** &mdash; most standard libraries are implemented in Fun itself
- **Internal style**: `snake_case` for functions and variables, `CamelCase` for class names

## The Fun Manifesto

Fun is a programming language built on a simple idea:
Coding should be enjoyable, elegant, and consistent.

### Philosophy

- **Fun is Fun**<br>
  Programming should spark creativity, not frustration. Code in Fun feels light, playful, and rewarding.
- **Fun Uses Nothing**<br>
  Minimalism is power. No unnecessary features, no endless syntax variations, no formatting debates. Just clean, uniform code.
- **Indentation is Truth**<br>
  Two spaces, always. No tabs, no four-space wars. Code should look the same everywhere, from your laptop to /usr/bin/fun.
- **One Way to Do It**<br>
  No clutter, no 15 ways of writing the same thing. Simplicity means clarity.
- **Hackable by Nature**<br>
  Fun should be small and embeddable, like Lua. Easy to understand, extend, and tinker with &mdash; true to the hacker spirit.
- **Beautiful Defaults**<br>
  A language that doesn't need linters, formatters, or style guides. Beauty is built in.

## The Community

Fun is not about being the fastest or the most feature-rich. It's about sharing joy in coding. The community should be:

- Respectful
- Curious
- Creative

Like the name says: Fun Unites Nerds.

Use the [Fun Community Page](../community/) to get in touch.

## The Goal

A language that feels like home for developers who:

- Love minimal, elegant tools
- Believe consistency is freedom
- Want to write code that looks good and feels good

Fun may not change the world &mdash; but it will make programming a little more fun.

## Language Features

### Core Syntax & Types
- Indentation-based blocks (2-space)
- Line and block comments
- Static type annotations with automatic runtime range clamping for fixed-width integers
- `typeof()`, `to_string()`, `to_number()`, `cast()` &mdash; type introspection and conversion
- `exit` statement with optional exit code
- Source file includes: `#include "path"` and `#include <path>`

### Operators
- Arithmetic: `+`, `-`, `*`, `/`, `%`
- Comparison: `<`, `<=`, `>`, `>=`, `==`, `!=`
- Logical: `&&`, `||`, `!` (short-circuit)
- Bitwise: `band()`, `bor()`, `bxor()`, `bnot()`, `shl()`, `shr()`, `rol()`, `ror()`
- Ternary: `condition ? true_expr : false_expr`

### Control Flow
- `if` / `else if` / `else`
- `while` with `break` and `continue`
- `for var in array` &mdash; iteration
- `for var in range(start, end)` &mdash; numeric range
- `for (key, value) in map` &mdash; map destructuring
- `match` expression (stdlib)
- `try` / `catch` / `finally`

### Functions
- `fun name(params) body` &mdash; named functions
- `fn(params) body` &mdash; anonymous function literals
- First-class: pass as arguments, store in variables
- Recursion support
- `return` with optional value (implicit nil)

### Object-Oriented Programming
- `class Name(typed params) body` &mdash; class definition
- `_construct(this, ...)` &mdash; auto-invoked constructor
- `this` keyword for instance access
- `obj.method(args)` &mdash; method call sugar (auto-binds this)
- `obj.field` &mdash; dot property access
- `class Child(...) extends Parent` &mdash; inheritance with method overriding

### Data Structures
- **Arrays**: literal `[1, 2, 3]`, index access `arr[0]`, slice `arr[1:3]`, negative indices, `len()`, `push()`, `pop()`, `insert()`, `remove()`, `contains()`, `indexOf()`, `clear()`, `enumerate()`, `zip()`, `join()`, `map()`, `filter()`, `reduce()`
- **Maps**: literal `{"key": value}`, bracket access `map["key"]`, dot access `map.key`, `has()`, `keys()`, `values()`
- **Strings**: concatenation with `+`, `len()`, `substr()`, `find()`, `split()`, `join()`

### Pattern Matching & Regex
- POSIX regex: `regex_match()`, `regex_search()`, `regex_replace()`

### Mathematics
- Built-in: `abs`, `min`, `max`, `fmin`, `fmax`, `clamp`, `pow`, `sqrt`, `floor`, `ceil`, `trunc`, `round`, `sin`, `cos`, `tan`, `exp`, `log`, `log10`, `gcd`, `lcm`, `isqrt`, `sign`
- Random: `random_seed()`, `random_int()`, `random_number()` (cryptographic)
- Integer clamping: `sclamp()`, `uclamp()`

### I/O & Platform
- `print()`, `echo()` &mdash; output
- `read_file()`, `write_file()` &mdash; file I/O
- `input_line()` &mdash; stdin with optional prompt
- `env()`, `env_all()` &mdash; environment variables
- `proc_run()`, `system()` &mdash; process execution
- `os_list_dir()` &mdash; directory listing

### Date, Time & Sleep
- `time_now_ms()`, `clock_mono_ms()`, `date_format()`, `sleep()`

### Networking (Built-in, Unix)
- **TCP**: listen, accept, connect, send, recv, close
- **Unix domain sockets**: listen, connect
- **Non-blocking I/O**: `fd_set_nonblock()`, `fd_poll_read()`, `fd_poll_write()`

### Serial Communication (Unix)
- `serial_open()`, `serial_config()`, `serial_send()`, `serial_recv()`, `serial_close()`

### Concurrency
- `thread_spawn(fn, args)` &mdash; spawn thread, returns ID
- `thread_join(id)` &mdash; join, returns result
- Cooperative async scheduler in stdlib

### Debugging
- Built-in debugger with 64 breakpoints
- Step, next, finish, continue modes
- `--trace` / `-t` for opcode-level execution tracing
- `--repl-on-error`: enter REPL on runtime error with stack preserved

## Standard Library (stdlib)

Written primarily in Fun itself (lib/):

- **String utilities**: trim, starts/ends-with, split, replace-all, case conversion, repeat
- **Array utilities**: slice, reverse, concat, unique, flatten
- **Math helpers**: `abs`, `clamp`, `gcd`, `lcm`, `powi`, min3, max3, array min/max
- **Hex encoding**: hex-to-bytes, bytes-to-hex, dec-to-hex, hex-to-dec
- **Base64 encoding**: encode/decode
- **Cryptography** (pure Fun): MD5, SHA-1, SHA-256, SHA-384, SHA-512, CRC-32, CRC-32C, AES-256 ECB
- **Functional utilities**: Option type (Some/None), Result type (Ok/Err), pattern matching
- **Range utilities**: `range(n)`, `range2(start, end)`, `range3(start, end, step)`
- **Date/Time**: `DateTime` class with formatting, timers, sleep
- **CLI**: `argv()`, `parse_args()` with flag support
- **Console**: `Console` class with prompt, ask, hidden input, progress bar
- **Process**: `Process` class wrapping proc_run/system
- **Thread**: `Thread` class wrapping spawn/join
- **Socket classes**: `TcpClient`, `TcpServer`, `UnixClient`
- **Serial**: `Serial` class
- **Async scheduler**: cooperative multitasking with I/O polling
- **HTTP Servers**: static file server, CGI-capable server
- **IRC**: `IRCClient` with full protocol support
- **CGI**: `CGI` class for web applications

## Optional Extensions (Build-time)

Enabled via CMake flags, each wrapping a mature C library:

- **[JSON](/documentation/extensions/json/)** (`FUN_WITH_JSON` / [json-c](https://json-c.github.io/json-c/){:class="ext"}) &mdash; parse, stringify, file I/O <span style="color:green;font-weight:bold;">&#9745;</span>
- **[cURL](/documentation/extensions/curl/)** (`FUN_WITH_CURL` / [libcurl](https://curl.se/libcurl/){:class="ext"}) &mdash; HTTP GET, POST, download <span style="color:green;font-weight:bold;">&#9745;</span>
- **[SQLite](/documentation/extensions/sqlite/)** (`FUN_WITH_SQLITE` / [libsqlite3](https://www.sqlite.org/){:class="ext"}) &mdash; open, query, exec, close <span style="color:green;font-weight:bold;">&#9745;</span>
- **[PCRE2](/documentation/extensions/pcre2/)** (`FUN_WITH_PCRE2` / [libpcre2](https://www.pcre.org/){:class="ext"}) &mdash; Perl-compatible regex with flags <span style="color:green;font-weight:bold;">&#9745;</span>
- **[OpenSSL](/documentation/extensions/openssl/)** (`FUN_WITH_OPENSSL` / [libcrypto](https://www.openssl.org/){:class="ext"}) &mdash; MD5, SHA-256, SHA-512, RIPEMD-160 hashing <span style="color:green;font-weight:bold;">&#9745;</span>
- **[INI](/documentation/extensions/ini/)** (`FUN_WITH_INI` / [iniparser](https://github.com/ndevilla/iniparser){:class="ext"}) &mdash; load, get/set, save config files <span style="color:green;font-weight:bold;">&#9745;</span>
- **[XML](/documentation/extensions/xml2/)** (`FUN_WITH_XML2` / [libxml2](http://xmlsoft.org/){:class="ext"}) &mdash; parse, navigate, query XML documents <span style="color:green;font-weight:bold;">&#9745;</span>
- **[PC/SC](/documentation/extensions/pcsc/)** (`FUN_WITH_PCSC` / [libpcsclite](https://pcsclite.apdu.fr/){:class="ext"}) &mdash; smart card communication <span style="color:green;font-weight:bold;">&#9745;</span>
- **[KCGI](/documentation/extensions/kcgi/)** (`FUN_WITH_KCGI` / [libkcgi](https://kristaps.bsd.lv/kcgi/){:class="ext"}) &mdash; CGI web applications <span style="color:green;font-weight:bold;">&#9745;</span>
- **[Redis/Valkey](/documentation/extensions/redis/)** (`FUN_WITH_REDIS` / [hiredis](https://github.com/redis/hiredis){:class="ext"}) &mdash; Redis and Valkey support <span style="color:green;font-weight:bold;">&#9745;</span>

<span style="color:green;font-weight:bold;">&#9745;</span> = Done

Each extension also has a corresponding stdlib wrapper class (e.g., `JSON`, `INI`, `XML`, `PCSC`, `PCRE2`, `KCGI`).

## Tooling

- **`fun`** &mdash; Interpreter/REPL: runs `.fun` scripts or starts interactive REPL
- **`funstx`** &mdash; Syntax checker with optional `--fix` mode
- **REPL** &mdash; Full-featured interactive shell with history, tab completion, multi-line input, and commands (`:help`, `:load`, `:edit`, `:save`, `:debug`, `:trace`, `:type`, and more)
- **Built-in debugger** &mdash; breakpoints, step/next/finish, stack traces
- **Formatting** &mdash; `clang-format` target for consistent C source

## Build System

- **CMake** 3.10+ with C99 standard
- Build toggles for debug mode, musl, REPL, C++ FFI, Rust FFI, Doxygen docs
- Per-extension toggles for all optional libraries
- Release builds with LTO, section GC, and symbol stripping
- CTest integration &mdash; example scripts run as automated tests
- Doxygen API reference generation

## Code

- [https://git.xw3.org/fun/fun/](https://git.xw3.org/fun/fun/){:class="git"}

