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
 * Rust FFI demo opcode with argument: OP_RUST_HELLO_ARGS
 * Pops a string from the stack and asks Rust to print it. Pushes Nil.
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
