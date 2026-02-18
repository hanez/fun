# Embedding Fun

This guide outlines how to embed the Fun VM in a host application and extend it from C/Rust.

## Overview
- The VM is implemented in C (see `src/vm/`).
- Optional Rust-based opcodes can be enabled via `FUN_WITH_RUST` (see `docs/rust.md`).

## Embedding from C
While the exact API surface may evolve, a typical embedding flow looks like:
1. Initialize the VM/runtime and allocate a context.
2. Load/compile Fun source or bytecode.
3. Push arguments or set globals as needed.
4. Execute entry function or script body.
5. Retrieve results and clean up.

See `src/vm/core` and related headers for public entry points and value types.

### Hosting considerations
- Threading: share VM state cautiously or create one VM per thread.
- Memory: clarify ownership of strings/buffers crossing the boundary.
- Errors: propagate parse/runtime errors back to the host with useful messages.

## Extending with Rust
When `FUN_WITH_RUST=ON`, a Rust static library from `src/rust/` is built and linked. You can:
- Implement new opcodes/functions in Rust.
- Expose a C ABI for the VM to call into.

See `docs/rust.md` for details and example code.
