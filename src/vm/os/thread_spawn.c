/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-09-30
 */

case OP_THREAD_SPAWN: {
    /* operand: 0 -> no args; 1 -> has args array or single arg */
    Value argsMaybe = make_nil();
    if (inst.operand == 1) {
        argsMaybe = pop_value(vm); /* maybe array or scalar */
    }
    Value fnv = pop_value(vm);
    int tid = fun_thread_spawn(fnv, argsMaybe, inst.operand == 1);
    free_value(fnv);
    if (inst.operand == 1) free_value(argsMaybe);
    push_value(vm, make_int(tid));
    break;
}
