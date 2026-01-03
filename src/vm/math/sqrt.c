/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-01-03
 */

/**
* @file sqrt.c
 * @brief Implements the OP_SQRT opcode using C99 math.h sqrt().
 */

#include <math.h>

case OP_SQRT: {
    Value v = pop_value(vm);
    if (v.type == VAL_INT || v.type == VAL_FLOAT) {
        double x = (v.type == VAL_FLOAT) ? v.d : (double)v.i;
        if (x < 0.0) {
            push_value(vm, make_float(NAN));
        } else {
            double r = sqrt(x);
            /* preserve int if exactly integral */
            if (r >= (double)INT64_MIN && r <= (double)INT64_MAX) {
                int64_t ii = (int64_t)r;
                if ((double)ii == r) {
                    push_value(vm, make_int(ii));
                } else {
                    push_value(vm, make_float(r));
                }
            } else {
                push_value(vm, make_float(r));
            }
        }
        free_value(v);
    } else {
        fprintf(stderr, "Runtime type error: SQRT expects number, got %s\n", value_type_name(v.type));
        exit(1);
    }
    break;
}
