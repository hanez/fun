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
* @file gcd.c
 * @brief Implements the OP_GCD opcode for greatest common divisor.
 */

case OP_GCD: {
    Value vb = pop_value(vm);
    Value va = pop_value(vm);
    if (!((va.type == VAL_INT) || (va.type == VAL_FLOAT)) ||
        !((vb.type == VAL_INT) || (vb.type == VAL_FLOAT))) {
        fprintf(stderr, "Runtime type error: GCD expects numbers, got %s and %s\n",
                value_type_name(va.type), value_type_name(vb.type));
        exit(1);
    }
    int64_t a = (va.type == VAL_INT) ? va.i : (int64_t)va.d;
    int64_t b = (vb.type == VAL_INT) ? vb.i : (int64_t)vb.d;
    if (a == INT64_MIN) a = (int64_t)INT64_MAX; else if (a < 0) a = -a;
    if (b == INT64_MIN) b = (int64_t)INT64_MAX; else if (b < 0) b = -b;
    while (b != 0) {
        int64_t t = a % b;
        a = b;
        b = t;
    }
    Value res = make_int(a);
    free_value(va);
    free_value(vb);
    push_value(vm, res);
    break;
}
