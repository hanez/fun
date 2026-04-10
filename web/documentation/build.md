---
layout: page
published: true
noToc: false
noComments: false
noDate: false
title: Fun - Building Fun
subtitle: How to build Fun with CMake, available targets, and build options (FUN_DEBUG, FUN_USE_MUSL, FUN_WITH_CPP, FUN_WITH_RUST, FUN_WITH_OPENSSL).
description: How to build Fun with CMake, available targets, and build options (FUN_DEBUG, FUN_USE_MUSL, FUN_WITH_CPP, FUN_WITH_RUST, FUN_WITH_OPENSSL).
permalink: /documentation/build/
lang: en
tags:
- available
- build
- building
- cmake
- fun_debug
- fun_use_musl
- fun_with_cpp
- fun_with_openssl
- fun_with_rust
- options
- targets
---


This guide describes how to build Fun from source using CMake and the available build options.

## Prerequisites
- A C compiler with C99 support
- CMake 3.20+ (or newer)
- Optional: Rust toolchain with cargo (required when building with `FUN_WITH_RUST=ON`)

## Common targets
- `build` - aggregate target that depends on `fun`, `fun_test`, and `test_opcodes`
- `fun` - the CLI executable
- `fun_test` - unit/feature tests (run with CTest)
- `test_opcodes` - opcode tests (executable)

These targets are defined by the project; use your configured CMake build directory/profile.

## Build options
Fun exposes several options you can toggle at configure time:

- `FUN_DEBUG` (ON/OFF) - Enables extra assertions and logging in the VM and runtime
- `FUN_USE_MUSL` (ON/OFF) - Link against musl for static/portable builds (Linux)
- `FUN_WITH_CPP` (ON/OFF) - Enable C++-based opcode/examples support
- `FUN_WITH_RUST` (ON/OFF) - Build and link Rust staticlib from `src/rust/`
- `FUN_WITH_OPENSSL` (ON/OFF) - Enable OpenSSL-backed helpers (MD5/SHA-256/SHA-512/RIPEMD-160)

### VM configuration constants
You can override internal VM limits at compile time by passing `-D<VAR>=<VALUE>` to CMake:

- `MAX_FRAMES` (default: 128) - Maximum depth of the call stack (frames)
- `MAX_FRAME_LOCALS` (default: 64) - Maximum number of local variables per frame
- `MAX_GLOBALS` (default: 128) - Maximum number of global variables
- `OUTPUT_SIZE` (default: 1024) - Size of the VM output buffer (number of values)
- `STACK_SIZE` (default: 1024) - Size of the VM evaluation stack (number of `Value` slots)

These are defined as `CACHE` variables, so they will persist in your `CMakeCache.txt`.

When configuring, the build prints a summary like:

```
==== Fun build options ====
  FUN_DEBUG: ENABLED|DISABLED
  FUN_USE_MUSL: ENABLED|DISABLED
  FUN_WITH_CPP: ENABLED|DISABLED
  FUN_WITH_RUST: ENABLED|DISABLED
===========================
```

## Example commands
Use the CLion-provided build directories or your own. Typical invocations:

### Debug
```
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug \
  -DFUN_DEBUG=ON -DFUN_WITH_RUST=OFF
cmake --build build --target build
```

### Release
```
cmake -S . -B build_release -DCMAKE_BUILD_TYPE=Release \
  -DFUN_DEBUG=OFF -DFUN_WITH_RUST=OFF
cmake --build build_release --target build
```

### Enabling optional extensions
```
cmake -S . -B build_release -DCMAKE_BUILD_TYPE=Release \
  -DFUN_WITH_CPP=ON -DFUN_WITH_RUST=ON -DFUN_WITH_OPENSSL=ON
cmake --build build_release --target build
```

### Customizing VM limits
```
cmake -S . -B build_custom -DSTACK_SIZE=4096 -DMAX_GLOBALS=512
cmake --build build_custom --target fun
```

If `FUN_WITH_RUST` is enabled, ensure `cargo` is available in PATH; the build will invoke it and link the produced static library.

If `FUN_WITH_OPENSSL` is enabled, CMake must detect your system OpenSSL (libcrypto).


## Running
- CLI: run the `fun` executable from your build directory.
- REPL: `fun -i` or just run `fun` without a script, depending on your CLI version (see [cli.md](./cli/)).
- Examples: see [examples.md](./examples/).

Tip: When running from the repository without installation, set `FUN_LIB_DIR` to the local `./lib` so includes can find the stdlib.
