/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file get_sp.c
 * @brief Implements the OP_RUST_GET_SP opcode (conditional build).
 *
 * Delegates to a Rust helper to read the VM's current stack pointer (sp).
 * When Rust support is disabled, this raises a runtime error and pushes -1 to
 * signal unavailability.
 */

/**
 * OP_RUST_GET_SP: () -> int | Nil
 *
 * Behavior (FUN_WITH_RUST=ON):
 * - Does not pop any values.
 * - Invokes fun_op_rget_sp(vm). The Rust helper may push the result or
 *   otherwise communicate it. The C side does not directly push here.
 *
 * Behavior (FUN_WITH_RUST=OFF):
 * - Raises a runtime error indicating missing Rust support.
 * - Pushes integer -1 as a sentinel value.
 */
case OP_RUST_GET_SP: {
#ifdef FUN_WITH_RUST
  extern int fun_op_rget_sp(VM * vm);
  int rc = fun_op_rget_sp(vm);
  (void)rc; /* rc currently unused; 0 means OK */
#else
  vm_raise_error(vm, "RUST_GET_SP requires FUN_WITH_RUST=ON at build time");
  push_value(vm, make_int(-1));
#endif
  break;
}
