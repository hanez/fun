# Writing Rust-backed opcodes for Fun VM

!!! CAUTION !!! I am not very experienced in using Rust. This should show that it is possible to implement opcodes in Rust and integrate them with the Fun VM.

This guide explains how to implement VM opcodes in Rust, wire them into the C VM, and use them from Fun scripts.

It assumes you are comfortable with basic Rust and C and have a working Fun checkout.

## Overview

Fun’s VM is written in C, but you can implement opcode handlers in Rust and call them via FFI. The typical flow is:

1) Write a Rust function with a stable C ABI (extern "C", #[no_mangle]) that takes a pointer to the VM and returns an int status code.
2) Use VM stack helpers (exposed to Rust via FFI) to pop arguments and push results.
3) Expose that Rust function to the C VM by calling it from the opcode dispatch (a case in the VM’s opcode switch or a small shim under src/vm/rust/).
4) Add or reuse a Fun builtin that maps to your opcode, then call it from Fun code.

## Project layout (relevant parts)

- src/rust/src/lib.rs — Rust library with exported opcode functions and FFI helpers.
- src/vm/rust/ — C-side wiring examples and small opcode cases calling into Rust.
- examples/rust_hello.fun — Example Fun script using a Rust-backed opcode.
- docs/opcodes.md — General overview of many built-in opcodes (mostly C-based).

## Enabling Rust in the build

Rust integration is optional and gated by a CMake flag. Default builds usually have it OFF.

Enable it for a configured profile (Debug or Release):

- Debug example:
  cmake -S . -B build_debug -DFUN_WITH_RUST=ON
  cmake --build build_debug --target fun

- Release example:
  cmake -S . -B build_release -DFUN_WITH_RUST=ON
  cmake --build build_release --target fun

Useful targets in this repository include:
- fun — the main executable
- rust_ops_build — helps build/link Rust ops when enabled
- test_opcodes — test executable (if you want to extend tests)

Note: In CLion, prefer building with one of the provided CMake profiles (Debug/Release) and avoid creating custom build directories.

## Writing an opcode in Rust

The Rust side is a no_std static library exposing C ABI functions that the VM can call. See src/rust/src/lib.rs for examples already in the tree.

Key points:
- Use extern "C" and #[no_mangle] to fix the symbol name.
- Take a raw pointer to the VM as *mut Vm; return i32 status (0 for success).
- Interact with the VM stack via helper FFI functions declared as externs.
- Provide a minimal panic handler (no_std) as shown in lib.rs.

Example: integer addition opcode implemented in Rust.

In src/rust/src/lib.rs:

    #![no_std]

    #[repr(C)]
    pub struct Vm;

    extern "C" {
        fn vm_pop_i64(vm: *mut Vm) -> i64;
        fn vm_push_i64(vm: *mut Vm, v: i64);
    }

    #[no_mangle]
    pub extern "C" fn fun_op_radd(vm: *mut Vm) -> i32 {
        unsafe {
            let b = vm_pop_i64(vm);
            let a = vm_pop_i64(vm);
            vm_push_i64(vm, a + b);
        }
        0
    }

    #[panic_handler]
    fn panic(_info: &core::panic::PanicInfo) -> ! { loop {} }

What this does:
- Pops two 64-bit integers from the VM stack.
- Pushes back their sum.
- Returns 0 to indicate success to the VM.

You can add more extern helpers (e.g., for strings, arrays, maps) once they are exposed by the C VM. The repository already includes a simple string example returning a const char* from Rust, see fun_rust_get_string() usage below.

## Wiring the opcode in C

To make the VM call your Rust opcode, add a small C-side case that invokes the exported Rust symbol. A minimal pattern lives under src/vm/rust/.

String demo wiring (already present): src/vm/rust/hello.c

    case OP_RUST_HELLO: {
    #ifdef FUN_WITH_RUST
        const char *s = fun_rust_get_string();
        if (!s) s = "";
        push_value(vm, make_string(s));
    #else
        vm_raise_error(vm, "RUST_HELLO requires FUN_WITH_RUST=ON at build time");
        push_value(vm, make_nil());
    #endif
        break;
    }

For a stack-based math opcode (like fun_op_radd), you would declare and call the Rust function similarly:

    #ifdef FUN_WITH_RUST
    extern int fun_op_radd(void* vm); // or use the proper VM type if available
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
