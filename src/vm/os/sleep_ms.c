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

case OP_SLEEP_MS: {
    Value ms = pop_value(vm);
    if (ms.type != VAL_INT) {
        fprintf(stderr, "Runtime type error: sleep(ms) expects Number (milliseconds)\n");
        free_value(ms);
        /* push Nil so caller-side POP is safe */
        push_value(vm, make_nil());
        break;
    }
    long t = (long)ms.i;
    if (t > 0) fun_sleep_ms(t);
    free_value(ms);
    /* push Nil so statement POP does not underflow */
    push_value(vm, make_nil());
    break;
}
