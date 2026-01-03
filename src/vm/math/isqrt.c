/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 *  Added: 2026-01-03
 */

/**
* @file isqrt.c
 * @brief Implements the OP_ISQRT opcode for integer square root (floor).
 */

case OP_ISQRT: {
    Value v = pop_value(vm);
    if (!((v.type == VAL_INT) || (v.type == VAL_FLOAT))) {
        fprintf(stderr, "Runtime type error: ISQRT expects number, got %s\n", value_type_name(v.type));
        exit(1);
    }
    int64_t a = (v.type == VAL_INT) ? v.i : (int64_t)v.d;
    if (a <= 0) {
        free_value(v);
        push_value(vm, make_int(0));
        break;
    }
    /* Binary isqrt inline */
    uint64_t n = (uint64_t)a;
    uint64_t x = 0;
    uint64_t bit = (uint64_t)1 << 62; /* highest even bit set */
    while (bit > n) bit >>= 2;
    while (bit != 0) {
        if (n >= x + bit) {
            n -= x + bit;
            x = (x >> 1) + bit;
        } else {
            x >>= 1;
        }
        bit >>= 2;
    }
    int64_t r = (int64_t)x;
    free_value(v);
    push_value(vm, make_int(r));
    break;
}
