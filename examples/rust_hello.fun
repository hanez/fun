#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-01-27
 */

/*
 * Rust-backed opcode demo: rust_hello()
 *
 * Build instructions:
 *   - Default builds disable Rust integration.
 *   - Enable it via: cmake -S . -B build_debug -DFUN_WITH_RUST=ON
 *   - Then: cmake --build build_debug --target fun
 *
 * Run:
 *   build_debug/fun examples/rust_hello.fun
 *
 * Expected output (with FUN_WITH_RUST=ON):
 *   Hello from Rust ops!
 *
 * If built without Rust, calling rust_hello() will raise a runtime error
 * explaining that Rust integration is disabled.
 */

print(rust_hello())

/* Expected output:
Hello from Rust ops!
*/ 
