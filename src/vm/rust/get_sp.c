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
 * Rust FFI demo opcode: OP_RUST_GET_SP
 * Calls into Rust to read vm.sp via raw pointer math and pushes it (int).
 */
case OP_RUST_GET_SP: {
#ifdef FUN_WITH_RUST
    extern int fun_op_rget_sp(VM *vm);
    int rc = fun_op_rget_sp(vm);
    (void)rc; /* rc currently unused; 0 means OK */
#else
    vm_raise_error(vm, "RUST_GET_SP requires FUN_WITH_RUST=ON at build time");
    push_value(vm, make_int(-1));
#endif
    break;
}
