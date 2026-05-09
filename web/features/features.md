---
layout: page
published: true
noToc: false
noComments: false
noDate: false
title: Fun — Feature Overview
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

This page summarizes the core capabilities of Fun: the language, its virtual machine/runtime, tooling, build options, and the surrounding ecosystem. For in‑depth pages, see also:

- VM opcodes overview: /documentation/opcodes/
- Optional extensions catalog: /documentation/extensions/

## Language

- Simple, expressive syntax designed for scripting and embedding
- First‑class functions and function calls
- Variables, locals and globals with stack‑based execution model
- Control flow: conditional jumps, returns, and basic boolean operators
- Arrays and maps as primary collection types (literal construction, indexing, slicing, membership)
- Strings with common operations (substring, find, split, join)
- Arithmetic and bitwise integer operations (add, sub, mul, div, mod; band, bor, bxor, shifts, rotations)
- Exceptions and error handling primitives (throw/try semantics at VM level)
- Regular expressions support via optional PCRE2 extension (see Extensions)

Notes:

- See the opcode index for the precise stack behavior of each operation: /documentation/opcodes/

## Virtual Machine & Runtime

- Compact stack‑based bytecode VM implemented in C (C99)
- Deterministic execution model with explicit opcodes for core language features
- Efficient array and map primitives with indexing and mutation opcodes
- Built‑in string operations and regex integration (with extension)
- Minimal error model integrated with VM (throw/try handlers)

## Standard Library (Core)

Core functionality available out of the box in the VM and library modules:

- Arrays: create, push/pop, insert/remove, slice, contains, join, enumerate
- Maps: create, has_key, keys, values
- Strings: substr, find, split, replace (with regex), join
- Math and bitwise ops over fixed‑width integers used by the VM

See the opcode list for the canonical reference: /documentation/opcodes/

## Tooling

- Fun interpreter/runtime executable (target: fun)
- REPL and small tools (targets: repl, funstx, examples under examples/)
- Test binaries (target: fun_test, test_opcodes)
- Formatting helper target (target: format) using clang‑format
- Make‑like aggregate build target (target: build)

## Build System & Options

Fun uses CMake and exposes toggles that mirror the project’s optionality at build time:

- FUN_DEBUG — enable extra diagnostics in builds
- FUN_USE_MUSL — build against musl when available
- FUN_WITH_CPP — enable C++ interop/components where applicable
- FUN_WITH_RUST — build and link an optional Rust static library with opcode examples
- FUN_BUILD_DOCS — generate documentation (Doxygen/website pipelines)
- FUN_WITH_* — per‑extension toggles (see Extensions section)

Targets available for CI/workflows include Experimental/Continuous/Nightly aggregates, plus unit tests and coverage helpers (see CMake targets).

## Portability

- Written in portable C99
- Designed to build on common Linux environments; musl support available

## Extensions (Optional, Build‑time)

Fun ships with optional integrations that can be enabled per environment. Highlights include:

- cURL (HTTP client)
- INI (iniparser)
- JSON (json‑c)
- libxml2 (XML)
- SQLite
- PCRE2 (Perl‑compatible regex)
- PC/SC (Smart cards)
- OpenSSL (crypto/TLS)
- kcgi (CGI/web helper)

See the full catalog with enable/requirement notes: /documentation/extensions/

## Web/CGI

- Optional kcgi integration for building CGI‑style programs and simple web endpoints (see Extensions)
- Website sources under web/ with Jekyll layouts; generated site in web/_site/

## Testing

- Unit and VM‑level opcode tests (targets: fun_test, test_opcodes)
- Example scripts under examples/ exercised by CI and manual runs

## Documentation

- Doxygen configuration for API references (Doxyfile)
- Human‑readable docs under web/documentation/ (opcodes, extensions, etc.)

## Licensing

- Licensed under an OSI‑approved license; see LICENSE at the repository root
