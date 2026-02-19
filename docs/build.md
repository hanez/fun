# Building Fun

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
- `FUN_WITH_LIBRESSL` (ON/OFF) - Enable LibreSSL-backed helpers (MD5/SHA-256/SHA-512/RIPEMD-160)

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
  -DFUN_WITH_CPP=ON -DFUN_WITH_RUST=ON -DFUN_WITH_OPENSSL=ON -DFUN_WITH_LIBRESSL=ON
cmake --build build_release --target build
```

If `FUN_WITH_RUST` is enabled, ensure `cargo` is available in PATH; the build will invoke it and link the produced static library.

If `FUN_WITH_OPENSSL` is enabled, CMake must detect your system OpenSSL (libcrypto).

If `FUN_WITH_LIBRESSL` is enabled, CMake detects LibreSSL directly (via pkg-config or standard include/lib locations) and links to LibreSSL's `libcrypto` — no OpenSSL installation is required. Both extensions use the EVP interface. Note: On OpenSSL 3.x, RIPEMD-160 may require the legacy provider; if unavailable, the helper returns an empty string.

## Running
- CLI: run the `fun` executable from your build directory.
- REPL: `fun -i` or just run `fun` without a script, depending on your CLI version (see [cli.md](./cli.md)).
- Examples: see [examples.md](./examples.md).

Tip: When running from the repository without installation, set `FUN_LIB_DIR` to the local `./lib` so includes can find the stdlib.
