    #endif

    case OP_RADD: {
    #ifdef FUN_WITH_RUST
        (void)fun_op_radd(vm);
    #else
        vm_raise_error(vm, "RADD requires FUN_WITH_RUST=ON at build time");
        push_value(vm, make_nil()); // or follow your opcode’s error convention
    #endif
        break;
    }

Notes:
- Follow the existing opcode conventions for your module (core, math, strings, etc.).
- If your build puts the Rust symbol into a static library, make sure the VM target links it when FUN_WITH_RUST is ON (the top-level CMake already does this for the examples provided).

## Using Rust-backed opcodes from Fun

Once wired, expose the opcode via a builtin function or directly in bytecode. The repository includes a demo builtin rust_hello() that returns a Rust-generated string.

Run the example:

1) Build with Rust enabled (Debug):
   cmake -S . -B build_debug -DFUN_WITH_RUST=ON
   cmake --build build_debug --target fun

2) Execute the script:
   build_debug/fun examples/rust_hello.fun

Expected output:
   Hello from Rust ops!

If you build without Rust, calling rust_hello() raises a runtime error indicating that Rust integration is disabled.

## Stack discipline and error handling

- Always pop exactly the arguments you expect and push exactly the results your opcode promises. Mismatch leads to stack corruption and hard-to-debug failures.
- Return an int status to the VM (0 for success). If your project uses a different convention for some opcodes, match it consistently.
- Validate types where appropriate (e.g., ensure values are integers before arithmetic). If a check fails, use the VM’s error mechanism (e.g., vm_raise_error) and follow the module’s convention on what to push after errors.

## Data types and FFI surface

The minimal helpers shown cover 64-bit integers and simple strings. Extending the Rust<->C bridge usually involves:
- Declaring additional extern "C" functions in Rust that the C VM implements (to read/write values on the stack, construct arrays/maps/strings, etc.).
- Ensuring all pointers and lifetimes are well-defined: strings pushed to the VM should be copied or allocated using VM facilities so they remain valid after the call.
- Keeping Rust no_std unless you add an allocator and link setup to support std.

## Troubleshooting

- Link errors: Make sure FUN_WITH_RUST=ON for your build directory and that the Rust library is compiled before linking the VM. Use the rust_ops_build target if provided by your profile.
- Missing symbol at runtime: Confirm #[no_mangle] and extern "C" on the Rust function and that C sees the correct prototype.
- Wrong or garbled values: Double-check stack order (Fun uses a stack VM; many ops pop in reverse order: first b, then a).
- No output from rust_hello(): Ensure you run a binary built with FUN_WITH_RUST=ON; otherwise the VM deliberately raises an error and returns Nil for that call.

## Small end-to-end checklist

1) Write the Rust function in src/rust/src/lib.rs with extern "C", #[no_mangle].
2) Use FFI helpers to pop arguments and push results.
3) Add a C-side case under src/vm/... (or src/vm/rust/...) that calls your Rust function when the opcode executes.
4) Ensure the build links Rust code when FUN_WITH_RUST=ON.
5) Add or reuse a builtin in the parser/runtime to surface your opcode to Fun code.
6) Build and run a small .fun example to validate behavior.

## References in this repo

- Rust lib with examples: src/rust/src/lib.rs
- C-side hello wiring: src/vm/rust/hello.c
- Demo script: examples/rust_hello.fun
- General opcode reference: docs/opcodes.md

## Links

Authoritative and practical resources on exposing Rust to C (FFI) and maintaining a C-compatible API:

- Rustonomicon: FFI overview and best practices
  https://doc.rust-lang.org/nomicon/ffi.html
- Rustonomicon: Calling Rust code from C
  https://doc.rust-lang.org/nomicon/ffi.html#calling-rust-code-from-c
- The Rust Book: Unsafe and FFI (extern, #[no_mangle], calling Rust from other languages)
  https://doc.rust-lang.org/book/ch19-01-unsafe-rust.html#calling-rust-functions-from-other-languages
- Rust Reference: extern blocks, ABIs, and linkage
  https://doc.rust-lang.org/reference/items/external-blocks.html
- Rust FFI Omnibus (examples for many patterns, including Rust ↔ C)
  https://github.com/shepmaster/rust-ffi-omnibus
- cbindgen (generate C headers from Rust libraries)
  https://cbindgen.github.io/cbindgen/
- bindgen (generate Rust bindings to existing C headers; useful when mixing C and Rust)
  https://github.com/rust-lang/rust-bindgen

## Return-only Rust string helper

Two variants are available for passing a string to Rust and getting output:

- rust_hello_args(msg)
  - Rust side prints the message to stdout; Fun receives Nil. Use when you only want side-effect printing.
- rust_hello_args_return(msg)
  - Rust side does not print; it returns the provided string to Fun (useful for assignment or chaining).

Example:

    msg = rust_hello_args_return("Hello back from Rust (no print)!")
    print(msg)

See examples/rust_hello_args_return.fun for a complete script.
