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
 * Rust-backed VM access demo:
 *   - rust_get_sp()   -> reads vm.sp (stack pointer) using raw pointer + offset from Rust
 *   - rust_set_exit(N) -> writes vm.exit_code = N from Rust
 */

print("[Rust VM access demo]")

/* Read vm.sp via Rust and print it */
print("vm.sp (from Rust) = " + to_string(rust_get_sp()))

/* Write vm.exit_code via Rust (not directly observable without VM inspection) */
print("Setting vm.exit_code to 42 via Rust...")
rust_set_exit(42)

print("Now exiting...")

/* Expected output:
[Rust VM access demo]
vm.sp (from Rust) = 0
Setting vm.exit_code to 42 via Rust...
Now exiting...
*/

// "echo $?" will show the exit code:
// 42
