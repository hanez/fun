/**
* This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

case OP_SCLAMP: {
    /* Two's complement wrap to signed N-bit range:
       - Mask to N bits
       - If sign bit is set, sign-extend to 64-bit
       This yields values in [-2^(N-1) .. 2^(N-1)-1]. */
    Value v = pop_value(vm);
    int bits = inst.operand;
    int64_t vi = (v.type == VAL_INT) ? v.i : 0;

    int64_t out = 0;
    if (bits <= 0) {
        out = 0;
    } else {
        uint64_t mask = (bits >= 64) ? UINT64_MAX : ((1ULL << bits) - 1ULL);
        uint64_t wrapped = ((uint64_t)vi) & mask;
        if (bits >= 64) {
            /* 64-bit: already full width; interpret as signed */
            out = (int64_t)wrapped;
        } else {
            uint64_t sign_bit = 1ULL << (bits - 1);
            if (wrapped & sign_bit) {
                /* sign-extend */
                out = (int64_t)(wrapped | (~mask));
            } else {
                out = (int64_t)wrapped;
            }
        }
    }

    push_value(vm, make_int(out));
    free_value(v);
    break;
}
