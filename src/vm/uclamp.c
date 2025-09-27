/**
* This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the ISC license.
 * https://opensource.org/license/isc-license-txt
 */

case OP_UCLAMP: {
    /* Clamp/wrap the integer on top of the stack to 'bits' width (unsigned) */
    Value v = pop_value(vm);
    int bits = inst.operand;
    uint64_t u = 0;

    if (v.type == VAL_INT) {
        u = (uint64_t) v.i;
    } else {
        /* Non-integers clamp to 0 */
        u = 0;
    }

    if (bits > 0 && bits < 64) {
        uint64_t mask = (1ULL << bits) - 1ULL;
        u &= mask;
    } else {
        /* bits >= 64 -> no-op; bits <= 0 -> no-op */
    }

    push_value(vm, make_int((int64_t)u));
    free_value(v);
    break;
}
