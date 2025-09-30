/**
* This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

case OP_UCLAMP: {
    /* Saturating clamp to unsigned N-bit range: [0 .. 2^N - 1] */
    Value v = pop_value(vm);
    int bits = inst.operand;
    int64_t vi = (v.type == VAL_INT) ? v.i : 0;

    uint64_t umax;
    if (bits <= 0) {
        /* treat as clamp to 0..0 */
        umax = 0;
    } else if (bits >= 64) {
        umax = UINT64_MAX;
    } else {
        umax = (1ULL << bits) - 1ULL;
    }

    uint64_t u;
    if (vi < 0) u = 0;
    else if ((uint64_t)vi > umax) u = umax;
    else u = (uint64_t)vi;

    push_value(vm, make_int((int64_t)u));
    free_value(v);
    break;
}
