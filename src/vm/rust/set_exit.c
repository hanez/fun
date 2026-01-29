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
 * Rust FFI demo opcode: OP_RUST_SET_EXIT
 * Pops an int code and asks Rust to write it into vm.exit_code.
 */
case OP_RUST_SET_EXIT: {
#ifdef FUN_WITH_RUST
    extern int fun_op_rset_exit(VM *vm);
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
