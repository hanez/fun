#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-01-29
 */

/*
 * Rust-backed opcode demo with argument: rust_hello_args_return(STRING)
 * This variant does NOT print from Rust; it only returns the provided string
 * back to Fun for assignment or further processing.
 *
 * Build instructions:
 *   - Enable Rust integration:
 *       cmake -S . -B build_debug -DFUN_WITH_RUST=ON
 *   - Then build:
 *       cmake --build build_debug --target fun
 *
 * Run:
 *   build_debug/fun examples/rust_hello_args_return.fun
 */

msg = rust_hello_args_return("Hello back from Rust (no print)!")
print(msg)

/* Expected output:
Hello back from Rust (no print)!
*/
