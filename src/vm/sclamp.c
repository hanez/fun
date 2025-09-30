/**
* This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

case OP_SCLAMP: {
    /* Saturating clamp to signed N-bit range: [-2^(N-1) .. 2^(N-1)-1] */
    Value v = pop_value(vm);
    int bits = inst.operand;
    int64_t vi = (v.type == VAL_INT) ? v.i : 0;

    int64_t smin, smax;
    if (bits <= 0) {
        smin = 0; smax = 0;
    } else if (bits >= 63) {
        /* cover full int64_t domain for 63+ bits */
        smin = INT64_MIN;
        smax = INT64_MAX;
    } else {
        smin = -(1LL << (bits - 1));
        smax =  (1LL << (bits - 1)) - 1LL;
    }

    if (vi < smin) vi = smin;
    else if (vi > smax) vi = smax;

    push_value(vm, make_int(vi));
    free_value(v);
    break;
}
