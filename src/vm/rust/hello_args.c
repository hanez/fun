/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file hello_args.c
 * @brief Implements the OP_RUST_HELLO_ARGS opcode (conditional build).
 *
 * Demonstrates passing a VM value to Rust. Converts the top stack value to a
 * string, calls into Rust to print/log it, and pushes Nil. When Rust support is
 * disabled, the argument is still popped and freed to keep the stack sane,
 * then Nil is pushed after raising an error message.
 * OP_RUST_HELLO_ARGS: (msg:any) -> Nil
 *
 * Behavior (FUN_WITH_RUST=ON):
 * - Pops one value from the stack and converts it to a newly allocated C
 *   string via value_to_string_alloc().
 * - Calls fun_rust_print_string(msg) and then frees the allocated buffer.
 * - Pushes Nil.
 *
 * Behavior (FUN_WITH_RUST=OFF):
 * - Pops and frees one value to preserve stack discipline.
 * - Raises a runtime error indicating missing Rust support.
 * - Pushes Nil.
 */

case OP_RUST_HELLO_ARGS: {
#ifdef FUN_WITH_RUST
  Value vmsg = pop_value(vm);
  char *msg = value_to_string_alloc(&vmsg);
  free_value(vmsg);
  if (msg) {
    fun_rust_print_string(msg);
    free(msg);
  } else {
    fun_rust_print_string("");
  }
  push_value(vm, make_nil());
#else
  /* Still pop and free the arg to keep stack sane */
  Value vmsg = pop_value(vm);
  free_value(vmsg);
  vm_raise_error(vm, "RUST_HELLO_ARGS requires FUN_WITH_RUST=ON at build time");
  push_value(vm, make_nil());
#endif
  break;
}
