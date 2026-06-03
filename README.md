# Fun ([https://fun-lang.xyz](https://fun-lang.xyz))

## What is Fun?

Fun is a small, strict, and simple programming language that runs on a compact stack-based virtual machine. The C core is intentionally minimal; most functionality and standard libraries are implemented in Fun itself.

Fun is dynamically typed with optional static type annotations, featuring first-class functions, classes with inheritance, pattern matching, and a rich standard library. It supports everything from basic scripting to TCP sockets, serial communication, threading, cryptography (MD5, SHA-1/256/384/512, CRC-32, AES-256), and a built-in debugger.

Fun is an experiment &mdash; just for fun &mdash; but Fun works!

Influenced by **[Bash](https://www.gnu.org/software/bash/)**, **[C](https://en.wikipedia.org/wiki/The_C_Programming_Language)**, **[Lua](https://www.lua.org/)**, PHP, **[Python](https://www.python.org/)**, and **[Rust](https://www.rust-lang.org/)**.

Fun is and will ever be 100% free under the terms of the [Apache-2.0 License](https://opensource.org/license/apache-2-0).

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
- Open for everyone

Like the name says: Fun Unites Nerds.

Please visit the [Fun Community Page](https://fun-lang.xyz/community/) to get in touch.

## The Goal

A language that feels like home for developers who:

- Love minimal, elegant tools
- Believe consistency is freedom
- Want to write code that looks good and feels good

Fun may not change the world &mdash; but it will make programming a little more fun.

## Language Features

### Core Syntax & Types

- Indentation-based blocks (2-space), line and block comments
- Static type annotations with automatic runtime range clamping for fixed-width integers
- `typeof()`, `to_string()`, `to_number()`, `cast()` &mdash; type introspection and conversion
- `exit` statement with optional exit code, `#include` for source includes

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
- First-class: pass as arguments, store in variables, recursion
- `return` with optional value (implicit nil)

### Object-Oriented Programming

- `class Name(typed params) body` with `_construct(this, ...)` auto-invoked constructor
- `this` keyword, `obj.method(args)` method call sugar, `obj.field` dot property access
- `class Child(...) extends Parent` with method overriding

### Data Structures

- **Arrays**: literal `[1, 2, 3]`, index `arr[0]`, slice `arr[1:3]`, negative indices; `len()`, `push()`, `pop()`, `insert()`, `remove()`, `contains()`, `indexOf()`, `clear()`, `enumerate()`, `zip()`, `join()`, `map()`, `filter()`, `reduce()`
- **Maps**: literal `{key: value}`, bracket `map["key"]`, dot `map.key` access; `has()`, `keys()`, `values()`
- **Strings**: concatenation with `+`, `len()`, `substr()`, `find()`, `split()`, `join()`

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
- `time_now_ms()`, `clock_mono_ms()`, `date_format()`, `sleep()`

### Networking (Built-in, Unix)

- TCP: listen, accept, connect, send, recv, close
- Unix domain sockets: listen, connect
- Non-blocking I/O: `fd_set_nonblock()`, `fd_poll_read()`, `fd_poll_write()`

### Serial Communication (Unix)

- `serial_open()`, `serial_config()`, `serial_send()`, `serial_recv()`, `serial_close()`

### Concurrency

- `thread_spawn(fn, args)` returns thread ID; `thread_join(id)` returns result
- Cooperative async scheduler in stdlib

### Debugging & Tooling

- Built-in debugger with 64 breakpoints, step/next/finish/continue
- `--trace` / `-t` for opcode-level execution tracing
- `--repl-on-error`: enter REPL on runtime error with stack preserved
- Full-featured REPL with history, tab completion, multi-line input, commands (`:help`, `:load`, `:edit`, `:save`, `:debug`, `:trace`, `:type`, and more)
- `funstx` &mdash; syntax checker with optional `--fix` mode

## Standard Library (lib/)

Written primarily in Fun itself:

- **Strings**: trim, starts/ends-with, split, replace-all, case conversion, repeat
- **Arrays**: slice, reverse, concat, unique, flatten
- **Math**: `abs`, `clamp`, `gcd`, `lcm`, `powi`, min3, max3, array min/max
- **Encoding**: hex encode/decode, base64 encode/decode
- **Cryptography** (pure Fun): MD5, SHA-1, SHA-256, SHA-384, SHA-512, CRC-32, CRC-32C, AES-256 ECB
- **Functional**: Option (Some/None), Result (Ok/Err), pattern matching
- **Ranges**: `range(n)`, `range2(start, end)`, `range3(start, end, step)`
- **Date/Time**: `DateTime` class with formatting, timers, sleep
- **CLI**: `argv()`, `parse_args()` with flag support
- **Console**: `Console` class with prompt, ask, hidden input, progress bar
- **Thread**: `Thread` class, **Process**: `Process` class
- **Socket classes**: `TcpClient`, `TcpServer`, `UnixClient`
- **Serial**: `Serial` class
- **Async**: Cooperative scheduler with I/O polling
- **HTTP**: Static file server, CGI-capable server
- **IRC**: `IRCClient` with full protocol support
- **CGI**: `CGI` class for web applications

## Optional Extensions (Build-time)

Enabled via CMake flags, wrapping mature C libraries:

| Extension    | Backend         |
|--------------|-----------------|
| JSON         | json-c          |
| cURL         | libcurl         |
| SQLite       | libsqlite3      |
| PCRE2        | libpcre2        |
| OpenSSL      | libcrypto       |
| INI          | iniparser 4.2.6 |
| XML          | libxml2         |
| PC/SC        | libpcsclite     |
| KCGI         | libkcgi         |
| Redis/Valkey | hiredis         |

Some extensions also have a corresponding stdlib wrapper class (e.g., `JSON`, `INI`, `XML`, `PCSC`, `PCRE2`, `KCGI`).

## Quick start

<pre>console
$ git clone https://git.xw3.org/fun/fun.git
$ cd fun
$ cmake -B build -DCMAKE_BUILD_TYPE=Release
$ cmake --build build
$ ./build/fun
fun> print("Hello, World!")
Hello, World!
fun> :quit</pre>

### OpenSSL quickstart (MD5)

See: [./web/documentation/extensions/openssl/openssl.md](./web/documentation/extensions/openssl/openssl.md)

## Build Options

- **`-DCMAKE_BUILD_TYPE=Debug`** &mdash; debug build with asserts
- **`-DCMAKE_BUILD_TYPE=Release`** &mdash; optimized build with LTO and stripping
- **`-DFUN_BUILD_MUSL=ON`** &mdash; static musl build
- Toggle each extension: `-DFUN_WITH_JSON=ON`, `-DFUN_WITH_CURL=ON`, etc.
- **`-DFUN_BUILD_DOXYGEN=ON`** &mdash; API reference

## Documentation

- Handbook: [./web/documentation/handbook/handbook.md](./web/documentation/handbook/handbook.md)
- Types: [./web/documentation/types/types.md](./web/documentation/types/types.md)
- REPL: [./web/documentation/repl/repl.md](./web/documentation/repl/repl.md)
- Testing: [./web/documentation/testing/testing.md](./web/documentation/testing/testing.md)
- Spec: [./web/documentation/spec/v0.4.md](./web/documentation/spec/v0.4.md)
- Changelog: [CHANGELOG.md](./CHANGELOG.md)
- Examples: [./examples/features.fun](./examples/features.fun)

## Author

Johannes Findeisen - <you@hanez.org>
