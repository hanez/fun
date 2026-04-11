---
layout: page
published: true
noToc: false
noComments: false
noDate: false
title: Fun - Documentation
subtitle: Detailed documentation for the Fun programming language.<br><br><span style="color:red;">The documentation is always a work in progress! It will always be behind the development of the code. It will be 100% aligned with a 1.0 release... ;)</span>
description: The Fun Documentation Index
permalink: /documentation/
lang: en
tags:
- index
- language
- programming
---


This file serves as an index of the documents in this directory. Links are relative and can be opened directly on Git hosting or locally.

## Basics

- [handbook/](./handbook/) - Comprehensive handbook for the Fun language and VM: install/build, configuration flags, usage, and full feature overview.
- [Fun REPL Guide](./repl/) - REPL guide: how to build/launch, editing and history, completions, REPL-on-error, and tips.
- [Specification v0.4](https://git.xw3.org/fun/fun/src/branch/main/spec/v0.4.md){:class="git"}
- [Specification v0.3](https://git.xw3.org/fun/fun/src/branch/main/spec/v0.3.md){:class="git"}
- [Specification v0.2](https://git.xw3.org/fun/fun/src/branch/main/spec/v0.2.md){:class="git"}
- [Specification v0.1](https://git.xw3.org/fun/fun/src/branch/main/spec/v0.1.md){:class="git"}
- [Examples](https://git.xw3.org/fun/fun/src/branch/main/examples){:class="git"}
- [Standard Library](https://git.xw3.org/fun/fun/src/branch/main/lib){:class="git"}

The examples directory contains demonstrations of most Fun features, from basic "Hello, World!" to threading, networking, classes, and more. The lib directory includes modules written in Fun itself.

## Overview

- [types/](./types/) - Core types (numbers, strings, arrays, maps, nil/bool), common operations, patterns, and interop notes.
- [numbers/](./numbers/) - Working with integers and floats: arithmetic, conversions, clamping, bitwise ops, and patterns.
- [strings/](./strings/) - Working with strings: literals/escaping, concatenation, substr/find, split, and conversions.
- [arrays/](./arrays/) - Working with arrays: creation, indexing/slicing, iteration patterns, helpers, and idioms.
- [maps/](./maps/) - Working with maps: construction, lookup/update, merging, iteration, and common patterns.
- [includes/](./includes/) - Using local vs. system includes, FUN_LIB_DIR, DEFAULT_LIB_DIR, and namespaced includes with `as`.
- [opcodes/](./opcodes/) - VM opcodes overview grouped by domain with brief behavior/stack notes.
- [internals/](./internals/) - Implementation details: bytecode format, VM architecture, stacks/frames, parser, and dispatch.
- [rust/](./rust/) - Writing Rust-backed opcodes and wiring them into the C VM; build/setup notes.
- [examples/](./examples/) - How to run the examples and the interactive showcase script, with environment tips.
- [testing/](./testing/) - How to build and run tests/targets with CMake/CTest, and where to add new tests.
- [troubleshooting/](./troubleshooting/) - Common issues and quick fixes for build, includes, and REPL usage.

## New and supplemental guides

- [build/](./build/) - How to build Fun with CMake, available targets, and build options (FUN_DEBUG, FUN_USE_MUSL, FUN_WITH_CPP, FUN_WITH_RUST, FUN_WITH_OPENSSL).
- [cli/](./cli/) - Command-line usage of the `fun` executable: synopsis, options, exit codes, includes and library paths.
- [fun/](./fun/) - Full usage guide for the `fun` executable: invocation patterns, REPL, env vars, include paths, examples, and install locations.
- [asyncio/](./asyncio/) - Async I/O primitives and patterns: non-blocking sockets, fd polling, examples, and best practices.
- [funstx/](./funstx/) - Syntax checker for .fun files with optional --fix auto-corrections; usage, exit codes, and limitations.
- [contributing/](./contributing/) - How to contribute: project structure, coding style, running tests, and PR guidelines.
- [style-guide/](./style-guide/) - Coding conventions for C and Fun (indentation, naming, idioms).
- [stdlib/](./stdlib/) - Overview of the standard library modules under ./lib with one-line summaries.
- [embedding/](./embedding/) - Embedding the VM from C/Rust, lifecycle, and host integration tips.
- [errors-and-diagnostics/](./errors-and-diagnostics/) - Understanding parser/runtime errors and enabling diagnostics.
- [performance/](./performance/) - Build/runtime tuning tips and patterns for better performance.
- [security-and-sandboxing/](./security-and-sandboxing/) - Trust boundaries, I/O expectations, and capability restrictions.
- [faq/](./faq/) - Frequently asked questions and quick answers.
- [website/](./website/) - Documentation for the [fun-lang.xyz](https://fun-lang.xyz) website in the `./web/` directory.
- [writing-tests/](./writing-tests/) - How to author new tests for Fun and opcode components.
- [bytecode-format/](./bytecode-format/) - Reference for the bytecode format (split out from internals for convenience).
- [roadmap/](./roadmap/) - High-level direction, planned features, and pointers to issues.

## Examples

 - [examples/](./examples/) - Catalog of all example scripts under [https://git.xw3.org/fun/fun/src/branch/main/examples](https://git.xw3.org/fun/fun/src/branch/main/examples){:class="git"}: what each area contains, how to run them, required env vars, and extension requirements.

## External extensions

Documentation for optional, build-time selectable integrations lives in [external/](./external/):

- [Index of extensions](./external/)
 - Highlights: [cURL](./external/curl/), [INI](./external/ini/), [JSON](./external/json/), [XML (libxml2)](./external/xml2/), [SQLite](./external/sqlite/), [PCRE2](./external/pcre2/), [PC/SC](./external/pcsc/), [OpenSSL](./external/openssl/)

## Tips

- When building from the repo without installing, set `FUN_LIB_DIR` to the local `./lib` directory so examples and the REPL can locate the stdlib.
- For a broader project overview and quickstart, see the repository root [README/](../../README/).
- Crypto examples:
  - If built with `-DFUN_WITH_OPENSSL=ON`, try `examples/crypto/openssl_md5.fun`.
