/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file hello_args_return.c
 * @brief Implements the OP_RUST_HELLO_ARGS_RETURN opcode (conditional build).
 *
 * Demonstrates round-tripping a string value through Rust. Pops a value,
 * converts it to a C string, passes it to Rust, and pushes back the string
 * returned by Rust. If Rust returns NULL or Rust support is disabled, Nil is
 * pushed.
 */

/**
 * OP_RUST_HELLO_ARGS_RETURN: (msg:any) -> string | Nil
 *
 * Behavior (FUN_WITH_RUST=ON):
 * - Pops one value from the stack and converts it to a newly allocated C
 *   string via value_to_string_alloc().
 * - Calls fun_rust_echo_string(msg) which returns an owned C string pointer
 *   that must be released via fun_rust_string_free().
 * - Pushes the returned string as VAL_STRING on success; pushes Nil if Rust
 *   returns NULL.
 *
 * Behavior (FUN_WITH_RUST=OFF):
 * - Pops and frees one value to preserve stack discipline.
 * - Raises a runtime error indicating missing Rust support.
 * - Pushes Nil.
 */
case OP_RUST_HELLO_ARGS_RETURN: {
#ifdef FUN_WITH_RUST
  Value vmsg = pop_value(vm);
  char *msg = value_to_string_alloc(&vmsg);
  free_value(vmsg);
  if (msg) {
    char *ret = fun_rust_echo_string(msg);
    free(msg);
    if (ret) {
      push_value(vm, make_string(ret));
      fun_rust_string_free(ret);
    } else {
      push_value(vm, make_nil());
    }
  } else {
    char *ret = fun_rust_echo_string("");
    if (ret) {
      push_value(vm, make_string(ret));
      fun_rust_string_free(ret);
    } else {
      push_value(vm, make_nil());
    }
  }
#else
  /* Still pop and free the arg to keep stack sane */
  Value vmsg = pop_value(vm);
  free_value(vmsg);
  vm_raise_error(vm, "RUST_HELLO_ARGS_RETURN requires FUN_WITH_RUST=ON at build time");
  push_value(vm, make_nil());
#endif
  break;
}
