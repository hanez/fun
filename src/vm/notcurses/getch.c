/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-01-03
 */

/* NC_GETCH */
case OP_NC_GETCH: {
    Value to_ms_v = pop_value(vm);
    int timeout_ms = (to_ms_v.type == VAL_INT ? (int)to_ms_v.i : (to_ms_v.type == VAL_FLOAT ? (int)to_ms_v.d : 0));
    free_value(to_ms_v);
#ifdef FUN_WITH_NOTCURSES
    if (_fun_nc) {
        /* For simplicity, use blocking get for now when timeout<=0 */
        if (timeout_ms <= 0) {
            ncinput ni;
            uint32_t id = notcurses_get_blocking(_fun_nc, &ni);
            push_value(vm, make_int((int)id));
        } else {
            /* Polling approximation: render then nonblocking get once */
            ncinput ni;
            uint32_t id = notcurses_get_nblock(_fun_nc, &ni);
            if (id == 0) {
                /* no input available now: return -1 to indicate timeout */
                push_value(vm, make_int(-1));
            } else {
                push_value(vm, make_int((int)id));
            }
        }
    } else {
        push_value(vm, make_int(-1));
    }
#else
    (void)_fun_nc; (void)_fun_nc_std; (void)timeout_ms;
    push_value(vm, make_int(-1));
#endif
    break;
}
