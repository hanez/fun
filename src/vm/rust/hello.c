/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-01-27
 */
 
 /**
 * Rust FFI demo opcode: OP_RUST_HELLO
 * When executed, it pushes a hello string returned by Rust onto the VM stack.
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
