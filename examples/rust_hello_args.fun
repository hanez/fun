#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-01-28
 */

/*
 * Rust-backed opcode demo with argument: rust_hello_args(STRING)
 *
 * Build instructions:
 *   - Rust integration is optional. Enable via:
 *       cmake -S . -B build_debug -DFUN_WITH_RUST=ON
 *   - Then build:
 *       cmake --build build_debug --target fun
 *
 * Run:
 *   build_debug/fun examples/rust_hello_args.fun
 *
 * Expected output (with FUN_WITH_RUST=ON):
 *   Hello from Fun to Rust!
 *
 * Note:
 *   The opcode prints from Rust and returns Nil, so there's nothing to print afterwards.
 */

rust_hello_args("Hello from Fun to Rust!")

/* Expected output:
Hello from Fun to Rust!
*/ 
