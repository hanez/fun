/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-09-30
 */

case OP_THREAD_JOIN: {
    Value vtid = pop_value(vm);
    if (vtid.type != VAL_INT) {
        fprintf(stderr, "Runtime type error: thread_join expects thread id (int)\n");
        push_value(vm, make_nil());
        free_value(vtid);
        break;
    }
    Value res = fun_thread_join((int)vtid.i);
    free_value(vtid);
    push_value(vm, res); /* takes ownership */
    break;
}
