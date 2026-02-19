# Fun Documentation Index

This file serves as an index of the documents in this directory. Links are relative and can be opened directly on Git hosting or locally.

## Overview

- [handbook.md](./handbook.md) - Comprehensive handbook for the Fun language and VM: install/build, configuration flags, usage, and full feature overview.
- [types.md](./types.md) - Core types (numbers, strings, arrays, maps, nil/bool), common operations, patterns, and interop notes.
- [numbers.md](./numbers.md) - Working with integers and floats: arithmetic, conversions, clamping, bitwise ops, and patterns.
- [strings.md](./strings.md) - Working with strings: literals/escaping, concatenation, substr/find, split, and conversions.
- [arrays.md](./arrays.md) - Working with arrays: creation, indexing/slicing, iteration patterns, helpers, and idioms.
- [maps.md](./maps.md) - Working with maps: construction, lookup/update, merging, iteration, and common patterns.
- [includes.md](./includes.md) - Using local vs. system includes, FUN_LIB_DIR, DEFAULT_LIB_DIR, and namespaced includes with `as`.
- [repl.md](./repl.md) - REPL guide: how to build/launch, editing and history, completions, REPL-on-error, and tips.
- [opcodes.md](./opcodes.md) - VM opcodes overview grouped by domain with brief behavior/stack notes.
- [internals.md](./internals.md) - Implementation details: bytecode format, VM architecture, stacks/frames, parser, and dispatch.
- [rust.md](./rust.md) - Writing Rust-backed opcodes and wiring them into the C VM; build/setup notes.
- [examples.md](./examples.md) - How to run the examples and the interactive showcase script, with environment tips.
- [testing.md](./testing.md) - How to build and run tests/targets with CMake/CTest, and where to add new tests.
- [troubleshooting.md](./troubleshooting.md) - Common issues and quick fixes for build, includes, and REPL usage.

## New and supplemental guides

- [build.md](./build.md) - How to build Fun with CMake, available targets, and build options (FUN_DEBUG, FUN_USE_MUSL, FUN_WITH_CPP, FUN_WITH_RUST, FUN_WITH_OPENSSL, FUN_WITH_LIBRESSL).
- [cli.md](./cli.md) - Command-line usage of the `fun` executable: synopsis, options, exit codes, includes and library paths.
- [contributing.md](./contributing.md) - How to contribute: project structure, coding style, running tests, and PR guidelines.
- [style-guide.md](./style-guide.md) - Coding conventions for C and Fun (indentation, naming, idioms).
- [stdlib.md](./stdlib.md) - Overview of the standard library modules under ./lib with one-line summaries.
- [embedding.md](./embedding.md) - Embedding the VM from C/Rust, lifecycle, and host integration tips.
- [errors-and-diagnostics.md](./errors-and-diagnostics.md) - Understanding parser/runtime errors and enabling diagnostics.
- [performance.md](./performance.md) - Build/runtime tuning tips and patterns for better performance.
- [security-and-sandboxing.md](./security-and-sandboxing.md) - Trust boundaries, I/O expectations, and capability restrictions.
- [faq.md](./faq.md) - Frequently asked questions and quick answers.
- [writing-tests.md](./writing-tests.md) - How to author new tests for Fun and opcode components.
- [bytecode-format.md](./bytecode-format.md) - Reference for the bytecode format (split out from internals for convenience).
- [roadmap.md](./roadmap.md) - High-level direction, planned features, and pointers to issues.

## External extensions

Documentation for optional, build-time selectable integrations lives in [external/](./external/):

- [Index of extensions](./external/README.md)
 - Highlights: [cURL](./external/curl.md), [INI](./external/ini.md), [JSON](./external/json.md), [XML (libxml2)](./external/xml2.md), [SQLite](./external/sqlite.md), [libSQL](./external/libsql.md), [PCRE2](./external/pcre2.md), [PC/SC](./external/pcsc.md), [Notcurses](./external/notcurses.md), [Tcl/Tk](./external/tcltk.md), [OpenSSL](./external/openssl.md), [LibreSSL](./external/libressl.md)

## Tips

- When building from the repo without installing, set `FUN_LIB_DIR` to the local `./lib` directory so examples and the REPL can locate the stdlib.
- For a broader project overview and quickstart, see the repository root [README.md](../README.md).
- Crypto examples:
  - If built with `-DFUN_WITH_OPENSSL=ON`, try `examples/crypto/openssl_md5.fun`.
  - If built with `-DFUN_WITH_LIBRESSL=ON`, try `examples/crypto/libressl_md5.fun`.
