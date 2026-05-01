/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file hello.c
 * @brief Implements the OP_RUST_HELLO opcode (conditional build).
 *
 * This opcode demonstrates calling into a Rust function from the VM. When
 * FUN_WITH_RUST is enabled at build time, it retrieves a greeting string from
 * Rust and pushes it as a VAL_STRING onto the VM stack. When Rust support is
 * disabled, a runtime error is raised and Nil is pushed to keep stack
 * consistency.
 * 
 * OP_RUST_HELLO: () -> string | Nil
 *
 * Behavior (FUN_WITH_RUST=ON):
 * - Does not pop any values.
 * - Calls fun_rust_get_string() and pushes the returned C string as a
 *   VAL_STRING. If Rust returns NULL, an empty string is used instead.
 *
 * Behavior (FUN_WITH_RUST=OFF):
 * - Raises a runtime error indicating missing Rust support.
 * - Pushes Nil to maintain stack discipline.
 */

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
