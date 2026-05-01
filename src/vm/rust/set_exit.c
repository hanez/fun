/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file set_exit.c
 * @brief Implements the OP_RUST_SET_EXIT opcode (conditional build).
 *
 * Pops an integer exit code and delegates to a Rust helper to store it into
 * vm.exit_code. Pushes Nil afterwards. When Rust support is disabled, the
 * argument is still popped/freed, a runtime error is raised, and Nil is
 * pushed to maintain stack discipline.
 * 
 * OP_RUST_SET_EXIT: (code:int) -> Nil
 *
 * Behavior (FUN_WITH_RUST=ON):
 * - Expects an integer on the stack (provided by previous opcodes).
 * - Calls fun_op_rset_exit(vm) which pops the int and writes vm->exit_code.
 * - Pushes Nil.
 *
 * Behavior (FUN_WITH_RUST=OFF):
 * - Pops and frees one value to preserve stack discipline.
 * - Raises a runtime error indicating missing Rust support.
 * - Pushes Nil.
 */

case OP_RUST_SET_EXIT: {
#ifdef FUN_WITH_RUST
  extern int fun_op_rset_exit(VM * vm);
  /* Expect an integer on the stack already (produced by prior ops) */
  /* fun_op_rset_exit will pop it and store into vm.exit_code */
  int rc = fun_op_rset_exit(vm);
  (void)rc;
  /* push Nil as a conventional result */
  push_value(vm, make_nil());
#else
  /* pop and ignore to keep stack sane */
  Value v = pop_value(vm);
  free_value(v);
  vm_raise_error(vm, "RUST_SET_EXIT requires FUN_WITH_RUST=ON at build time");
  push_value(vm, make_nil());
#endif
  break;
}
