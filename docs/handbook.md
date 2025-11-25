# Fun Handbook

## Overview

## Introduction

## Installation

### Requirements

A C compiler, a libc and [Git](https://git-scm.com/).

#### FreeBSD

- [CMake](https://cmake.org/)
- [Clang](https://clang.llvm.org/)

#### Linux

- [CMake](https://cmake.org/)
- [GCC](https://gcc.gnu.org/) (Clang should work here too, not tested!)

#### Windows

This requires Cygwin to be installed and configured. I will not cover this here.

- [CMake](https://cmake.org/)
- [Cygwin](https://cygwin.com/) using [GCC](https://gcc.gnu.org/)

### Build Fun

Linux/UNIX and Cygwin only covered here for now.

Clone repository:

```bash
git clone https://git.xw3.org/fun/fun.git
```

Change directory:

```bash
cd fun
```

Build:

```bash
# Note: Every -D flag must be of the form NAME=VALUE (e.g., -DFUN_WITH_REPL=ON)
cmake -S . -B build -DFUN_DEBUG=OFF -DFUN_WITH_PCSC=OFF -DFUN_WITH_REPL=ON -DFUN_WITH_JSON=ON -DFUN_WITH_PCRE2=ON
cmake --build build --target fun
```

That's it! For testing it, run:

```bash
FUN_LIB_DIR="$(pwd)/lib" ./build/fun ./demo.fun
```

To see what's going on, run:

```bash
FUN_LIB_DIR="$(pwd)/lib" ./build/fun --trace ./demo.fun
```

To switch into the REPL after an error, run:

```bash
FUN_LIB_DIR="$(pwd)/lib" ./build/fun --repl-on-error --trace ./demo.fun
```

Both --repl-on-error and --trace are optional but can always be combined. To get
more debug information, you need to build Fun with -DFUN_DEBUG=ON.

To directly run the REPL, you have to run:

```bash
FUN_LIB_DIR="$(pwd)/lib" ./build/fun
```

But be sure to build Fun with -DFUN_WITH_REPL=ON.

#### CMake options

All CMake options must be passed as -DNAME=VALUE:

- FUN_DEBUG=ON|OFF — verbose debug logging in the VM (default OFF)
- FUN_WITH_CURL=ON|OFF — enable CURL support using libcurl (default OFF)
- FUN_WITH_JSON=ON|OFF — enable JSON support via json-c (default OFF)
- FUN_WITH_PCRE2=ON|OFF — enable PCRE2 Perl-Compatible Regular Expressions support (default OFF)
- FUN_WITH_PCSC=ON|OFF — enable PCSC smart card support (default OFF)
- FUN_WITH_REPL=ON|OFF — enable the interactive REPL (default ON)

If you encounter an error such as:

  CMake Error: Parse error in command line argument: FUN_WITH_JSON
  Should be: VAR:type=value

then a -D option was given without a value. Always use -DNAME=VALUE, for example -DFUN_WITH_JSON=ON.

### Install Fun to OS

I do not recommend installing Fun on your system because it is in a very early
stage of development, but I can say that I have Fun installed on my system. If
you want to do that too, type:

```bash
sudo cmake --build build --target install
```

Now run Fun without prefixed FUN_LIB_DIR="$(pwd)/lib" because libs are installed to the
system default lib directory (/usr/share/fun/lib).

## Usage

```bash
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
  - PCSC (PC/SC smart card)
- Examples reference (what each example does)

---

## Language overview and VM internals

Fun is a small imperative language executed by a register-less stack-based virtual machine (VM). Source files are compiled to bytecode; the VM executes opcodes that work on a value stack. Functions and methods push/pop their arguments and return values on that stack.

High-level architecture:
- Front-end: parses .fun files, handles includes and constant folding, emits bytecode with debug markers (OP_LINE) for tracing and REPL-on-error.
- VM core: runs a loop over opcodes (see src/bytecode.h). Values include numbers (integers), strings, arrays, maps, booleans (0/1), functions, and nil.
- Built-ins: I/O, strings, arrays, regex, date/time, OS, networking, threading, optional JSON and PC/SC. Many are exposed as opcodes with friendly global functions in the language.

Key VM concepts (non-exhaustive):
- Control flow: OP_JUMP, OP_JUMP_IF_FALSE, OP_RETURN.
- Arithmetic and logic: OP_ADD/SUB/MUL/DIV, OP_MOD, comparisons (OP_LT, OP_LTE, OP_GT, OP_GTE, OP_EQ, OP_NEQ), logical OP_AND/OR/NOT.
- Stack helpers: OP_DUP, OP_SWAP, OP_POP.
- Arrays: OP_MAKE_ARRAY, OP_INDEX_GET/SET, OP_LEN, OP_PUSH, OP_APOP (pop last), OP_INSERT/REMOVE, OP_SLICE.
- Strings: OP_SUBSTR, OP_SPLIT, OP_JOIN, OP_FIND.
- Maps: OP_MAKE_MAP and index ops reuse array/map machinery; you can index with string keys.
- Conversion and typing: OP_TO_NUMBER, OP_TO_STRING, OP_CAST, OP_TYPEOF, unsigned/signed clamps (OP_UCLAMP/OP_SCLAMP).
- Regex: OP_REGEX_MATCH/SEARCH/REPLACE.
- Math misc: OP_MIN/MAX/CLAMP/ABS/POW/RANDOM_SEED/RANDOM_INT.
- Iteration helpers: OP_ENUMERATE, OP_ZIP.
- OS/IO/network: socket ops, file ops, process execution, environment, threads, etc., implemented as built-ins.
- Optional features: JSON opcodes (OP_JSON_PARSE and friends in src/vm/json/*) are compiled in only if -DFUN_WITH_JSON=ON. PCSC opcodes are available if -DFUN_WITH_PCSC=ON.

Error handling and debugging:
- Build with FUN_DEBUG=ON for verbose VM traces.
- Run with --trace to print executed lines and opcodes.
- Run with --repl-on-error to drop into an interactive REPL when a runtime error occurs, allowing inspection of variables and stepping.

## Command line interface and REPL

Running a script:
- fun path/to/script.fun
- Options: --trace, --repl-on-error (can combine), see build section for REPL availability.

REPL:
- Launch with fun (no script) when built with FUN_WITH_REPL=ON.
- In trace/REPL-on-error mode, the VM annotates output with file:line and function names to aid debugging (see examples/debug_reporting.fun and examples/repl_on_error.fun).

## Core types and operations

Types:
- number: signed integer. Conversions: to_number(x). Bitwise ops exist via bnot, band, bor, bxor, shl, shr, rol, ror in stdlib/VM.
- string: immutable sequence of bytes; length via len(s); concatenate via join([a,b], ""). Substring: substr(s, start, len). Find: find(haystack, needle) returns index or -1.
- array: ordered list. Create with [a, b, c] or built-ins. len(a), push(a, v) appends, apop(a) removes last, insert(a, idx, v), remove(a, idx), slice(a, start, end).
- map: associative dictionary with string keys typically: m = {}; m["key"] = value; keys can be strings and sometimes numbers.
- boolean: represented as number 1 (true) or 0 (false). Logical operators: &&, ||, !.
- nil: absence of value. Many defensive stdlib wrappers return [] or {} or nil defaults on errors.

Control flow:
- if/else, while loops, for-like range utilities (see utils.range in stdlib), break/continue (see examples/loops_break_continue.fun).

Functions and classes:
- Define a function with fun name(args) ...
- Define a class with class Name(constructor params) and methods fun method(this, ...) ...; _construct is called as a constructor if present.
- Methods use explicit this.

Modules and includes:
- Use #include <path/to/module.fun> to include from FUN_LIB_DIR.
- Use #include "relative/path.fun" to include a file relative to your script.
- You can alias includes with "as" to create namespaces: #include <utils/math.fun> as m; then call m.add(...).

## Built-ins overview

Console and I/O:
- print(x): prints a value with a trailing newline. input(prompt): returns a line as string without trailing newline.

Strings and arrays:
- len(x), join(array, sep), split(string, sep), substr(string, start, len), find(haystack, needle), push(array, value), apop(array), insert(array, idx, value), remove(array, idx), slice(array, start, end).

Conversion and type:
- to_number(x), to_string(x), cast(value, typeName), typeof(x), uclamp(number, bits), sclamp(number, bits).

Math and random:
- min(a,b), max(a,b), clamp(x, lo, hi), abs(x), pow(a,b), random_seed(seed), random_int(lo, hiExclusive).

Regex:
- regex_match(text, pattern) -> 1/0 full match
- regex_search(text, pattern) -> map {"match", "start", "end", "groups"}
- regex_replace(text, pattern, repl) -> string with global replacements

OS and processes:
- proc_run(cmd) -> map {"out": string, "code": number}
- system(cmd) -> exit code number
- env_get(name)/env_set(name, value) – see examples/os_env.fun

Networking and sockets:
- tcp_connect(host, port) -> fd (>0) or 0
- sock_send(fd, string) -> bytes sent or -1, sock_recv(fd, maxlen) -> string, sock_close(fd)
- tcp_listen(port, backlog) -> listen fd, tcp_accept(listenFd) -> client fd
- unix_connect(path) -> fd for UNIX domain sockets

Threads:
- thread_spawn(func, args) -> thread id; thread_join(id) -> return value

Date and time:
- time_now_ms() -> epoch ms; clock_mono_ms() -> monotonic ms; date_format(ms, fmt) -> string

JSON (optional):
- json_parse(text) -> Fun value (maps/arrays/numbers/strings/1/nil)
- json_stringify(value, prettyFlag) -> string; prettyFlag: 0/1
- json_from_file(path) -> value or nil; json_to_file(path, value, prettyFlag) -> 1/0

PC/SC (optional):
- pcsc_establish() -> context id (>0) or 0
- pcsc_list_readers(ctx) -> array of reader names (strings) or nil
- pcsc_connect(ctx, readerName) -> handle id (>0) or 0
- pcsc_disconnect(handle) -> 1/0
- pcsc_transmit(handle, bytesArray) -> map {"data": array of numbers, "sw1": n, "sw2": n, "code": n}

Note: Optional feature availability depends on your CMake flags at build time.

---

## Standard library APIs

The stdlib provides small, defensive wrappers around VM built-ins, typically with class-based APIs to avoid global name collisions and to offer sensible defaults.

### io.console

Class Console (lib/io/console.fun):
- prompt(text) -> string: print text and read a line.
- ask(question) -> string: prints "question: " and reads a line.
- ask_yes_no(question) -> 1/0: loops until user answers y/yes or n/no (case-insensitive).

Example:
- See examples/input_example.fun

### io.process

Class Process (lib/io/process.fun):
- run(cmd) -> { out, code }: captures stdout and exit code.
- run_merge_stderr(cmd) -> { out, code }: appends "2>&1" to merge stderr.
- system(cmd) -> number: exit code.
- check_call(cmd) -> 1/0: 1 if exit code is 0.

Examples:
- examples/process_example.fun

### io.socket

Provides TcpClient, TcpServer, UnixClient (lib/io/socket.fun).

Class TcpClient:
- connect(host, port) -> 1/0
- is_connected() -> 1/0
- send(data) -> bytes or -1
- recv(maxlen) -> string
- recv_all(chunk_size) -> string: keeps reading until EOF or partial chunk.
- close() -> 1

Class TcpServer(port, backlog):
- listen() -> listen fd or 0
- accept() -> client fd
- echo_once(maxlen) -> 1 on handled client
- serve_forever(maxlen) -> never returns; minimal echo server
- close()

Class UnixClient:
- connect(path), is_connected(), send(data), recv(maxlen), close()

Examples:
- examples/tcp_http_get.fun, examples/tcp_http_get_class.fun, examples/unix_socket_echo.fun, examples/extra/tcp_echo_server_class.fun

### io.thread

Class Thread (lib/io/thread.fun):
- spawn(func, args) -> thread id; join(id) -> return value
- Aliases: start(func, args), wait(id)

Examples:
- examples/threads_demo.fun, examples/thread_class_example.fun

### utils.datetime

Class DateTime (lib/utils/datetime.fun):
- now_ms() -> current epoch milliseconds
- mono_ms() -> monotonic clock ms
- format(ms, fmt) -> string using strftime-like fmt
- iso_now() -> "YYYY-MM-DDTHH:MM:SS"

Example:
- examples/datetime_basic.fun

### regex

Class Regex (lib/regex.fun):
- match(text, pattern) -> 1/0 full match
- search(text, pattern) -> map { match, start, end, groups }
- replace(text, pattern, repl) -> string (global)

Examples:
- examples/regex_demo.fun, examples/regex_procedural.fun

### crypt

MD5 (lib/crypt/md5.fun): Pure Fun implementation with class MD5 and helper md5_hex(hexStr). See examples/md5_demo.fun.

SHA family (lib/crypt/sha1.fun, sha256.fun, sha384.fun, sha512.fun): class wrappers SHA1/SHA256/SHA384/SHA512 with methods digest_hex_of_string(str) and helpers as documented in files. Examples: sha1_demo.fun, sha256_demo.fun, sha256_str_demo.fun, sha384_example.fun, sha512_demo.fun, sha512_str_demo.fun.

### encoding.base64

Module lib/encoding/base64.fun provides base64_encode(string) and base64_decode(string) helpers (see file for exact APIs). Used in some examples.

### arrays, strings, maps helpers

- lib/arrays.fun: helper functions for common array patterns.
- lib/strings.fun: string helpers like str_to_lower/upper and more; used by several stdlib modules.
- lib/hex.fun: bytes_to_hex(arrayOfNumbers) and hex_to_bytes(hexString) helpers as used by PCSC.
- lib/utils/range.fun: utilities for building numeric ranges; see for_range_test.fun.
- lib/utils/math.fun and lib/math.fun: higher-level math helpers.

---

## Extra libraries

### JSON (optional)

Build flag: -DFUN_WITH_JSON=ON. Requires json-c available on your system. Internals are in src/vm/json/ and wrap json-c to convert between json_object and Fun values.

VM functions:
- json_parse(text) -> value or nil on parse error.
- json_stringify(value, pretty) -> string; pretty is 0/1.
- json_from_file(path) -> value or nil if file missing/unreadable.
- json_to_file(path, value, pretty) -> 1 on success else 0.

Stdlib wrapper class JSON (lib/io/json.fun):
- parse(text)
- stringify(value, pretty=0)
- from_file(path)
- to_file(path, value, pretty=0)

Example walkthrough (examples/json_showcase.fun):
- Parses a JSON string into a map/array structure; demonstrates indexing (obj["name"]).
- Pretty prints the object with json.stringify(obj, 1).
- Attempts to read a non-existent file to show defensive behavior.
- Loads examples/data/complex.json, accesses nested fields, constructs a summary map, and writes pretty JSON to /tmp.

### PCSC (optional)

Build flag: -DFUN_WITH_PCSC=ON. Requires PC/SC (e.g., pcsc-lite on Unix) and a reader. VM opcodes are wrapped by global functions as listed under Built-ins.

Stdlib wrapper class PCSC (lib/io/pcsc.fun):
- get_readers() -> array of reader names.
- transmit(hex_apdu) -> map result by establishing context, selecting a reader, connecting, transmitting, and disconnecting. It returns a map with keys data (array), sw1, sw2, code. The wrapper includes defensive defaults when no reader exists.
- There is also a commented-out full-featured variant exposing establish/release/connect/disconnect/transmit_bytes/transmit_hex for advanced use.

Example:
- examples/pcsc_example.fun: shows establishing and transmitting an APDU, or printing []/default map if no readers present.

---

## Examples reference

You can run examples without installing by pointing FUN_LIB_DIR to the repository lib directory:

  FUN_LIB_DIR="$(pwd)/lib" ./build/fun examples/<name>.fun

Below is a catalog of the examples folder with brief explanations of what happens in each file:

- arrays.fun — Basic array creation, indexing, push/apop, insert/remove, slice; prints intermediate states and lengths.
- arrays_advanced.fun — More complex array transformations, enumerate/zip patterns.
- arrays_iter.fun — Iterating arrays with indices and values; demonstrates for/while patterns.
- boolean_decl.fun — Declaring and using booleans, truthy/falsey checks.
- booleans.fun — First-class booleans with logical operators and short-circuit behavior.
- builtins_conversions.fun — Using to_number, to_string, cast, typeof, uclamp/sclamp.
- builtins_extended.fun — Showcases extended built-ins like min/max/clamp/abs/pow/random.
- builtins_maps_and_more.fun — Demonstrates map creation, assignment, and index operations.
- byte_for_demo.fun — Demonstrates bitwise ops (bnot/band/bor/bxor/shl/shr/rol/ror) and numeric behavior.
- cast_demo.fun — Casting values and type checking via typeof and cast.
- class_constructor.fun — Using _construct in classes and field initialization.
- classes_demo.fun — Class definition, methods, and instances interacting.
- datetime_basic.fun — Uses utils.datetime to print now_ms, mono_ms, and formatted timestamps.
- debug_reporting.fun — Shows how --repl-on-error and trace annotate crashes with file:line and stack info.
- exit_example.fun — Demonstrates exiting a program early and exit codes.
- expressions_test.fun — Demonstrates operator precedence and expression evaluations.
- fail.fun — Purposefully triggers an error to see runtime behavior.
- file_io.fun — Reading/writing files with built-ins; prints file contents.
- file_print_for_file_line_by_line.fun — Iterates through file lines, printing them.
- floats.fun — Demonstrates float-like operations if represented via numbers; shows division behavior.
- for_range_test.fun — Uses utils.range to iterate over numeric ranges.
- functions_test.fun — Function definitions, higher-order usage, and composition.
- have_fun.fun — A fun greeting and minimal example to verify environment.
- have_fun_function.fun — Extracted function used by have_fun.fun.
- if_else_test.fun — Conditional branching and nesting.
- include_lib.fun — Using #include <...> from FUN_LIB_DIR.
- include_local.fun — Using #include "..." relative path includes and shared helpers.
- include_namespace.fun — Namespaced includes with "as" and usage examples.
- inheritance_demo.fun — Class inheritance patterns and method overriding.
- input_example.fun — Reading from stdin using Console.ask/prompt.
- json_showcase.fun — Comprehensive demo of JSON.parse/stringify/from_file/to_file; prints nested values and writes to /tmp.
- loops_break_continue.fun — Shows break and continue in loops and their effects on control flow.
- md5_demo.fun — Hashing data using lib/crypt/md5.fun and printing the digest.
- namespaced_mod.fun — Module used by include_namespace.fun to demonstrate namespacing.
- nested_loops.fun — Nested iteration and control flow.
- objects_basic.fun — Creating and manipulating maps as objects with fields.
- objects_more.fun — More advanced object/map patterns.
- os_env.fun — Getting/setting environment variables.
- pcsc_example.fun — Establishing PC/SC context, listing readers, transmitting a sample APDU if hardware present.
- process_example.fun — Running external commands with Process.run/system and handling exit codes.
- regex_demo.fun — Using Regex class for match/search/replace; prints results and groups.
- regex_procedural.fun — Direct usage of regex_* built-ins without the class wrapper.
- repl_on_error.fun — Forces an error to enter REPL when run with --repl-on-error.
- sha1_demo.fun — Hashing using SHA1 helper; prints digest.
- sha256_demo.fun — SHA-256 hashing demonstration over file/string inputs.
- sha256_str_demo.fun — String-only SHA-256 hashing convenience.
- sha384_example.fun — SHA-384 hashing demonstration.
- sha512_demo.fun — SHA-512 hashing demonstration over data; prints digest.
- sha512_str_demo.fun — String-only SHA-512 hashing convenience.
- short_circuit_test.fun — Demonstrates && and || short-circuit semantics.
- signed_ints.fun — Two's complement wrapping and signed integer behavior.
- stdlib_showcase.fun — A tour of several stdlib modules in one file.
- strings_test.fun — String slicing, joining, splitting, find, and case transforms.
- tcp_http_get.fun — Minimal HTTP GET over TCP using built-ins; prints the response.
- tcp_http_get_class.fun — Same as above using the TcpClient class.
- thread_class_example.fun — Spawning and joining threads via the Thread class methods.
- threads_demo.fun — Multiple threads and returning values with thread_join.
- try_catch_finally.fun — Error handling with try/catch/finally constructs.
- try_catch_with_error.fun — Catching and inspecting errors thrown inside code.
- typeof_features.fun — Shows typeof on many values and casting behavior.
- typeof.fun — Basic typeof usage.
- type_safety_fails.fun — Examples that should fail type safety checks at runtime.
- type_safety.fun — Properly typed examples that run without errors.
- types_integers.fun — Integer type features, comparisons, and arithmetic.
- types_overview.fun — Overview of values and literal syntax.
- uint_types.fun — Unsigned integer helpers and clamping.
- unix_socket_echo.fun — UNIX domain socket echo client/server demo.
- while_test.fun — While loops, counters, and loop termination conditions.

Notes:
- Some examples are platform-dependent (PCSC, UNIX sockets) or rely on optional features (JSON). They degrade gracefully when unavailable, printing empty arrays or default maps.

---

## Internals notes for JSON

Fun wraps json-c. See src/vm/json/parse.c, stringify.c, from_file.c, to_file.c. For parsing, OP_JSON_PARSE converts the input string into a json_object using a tokener and then converts to Fun values via json_to_fun. On error or when JSON is compiled out, the VM returns nil. The stdlib JSON class converts arguments defensively (to_string) and provides default pretty=0.

## Internals notes for PCSC

The PCSC functions in the VM interface with pcsc-lite/WinSCard. Transmit returns a map with raw data bytes and status words (sw1, sw2) and a code field. The stdlib wrapper in lib/io/pcsc.fun demonstrates defensive patterns: when no readers are found, it returns a default map so that indexing like res["sw1"] is always safe.

---

## Contributing and further reading

- Browse lib/ for up-to-date stdlib APIs; many files document their own public interfaces in comments at the top.
- src/bytecode.h lists all opcodes supported by the VM. The corresponding implementations live under src/vm/.
- examples/ are the best starting point to learn by doing.

