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
 * Rust FFI demo opcode with argument: OP_RUST_HELLO_ARGS_RETURN
 * Pops a string from the stack, asks Rust to return it back (no printing).
 * Pushes the returned string, or Nil on error.
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
