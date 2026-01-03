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
* @file ceil.c
 * @brief Implements the OP_CEIL opcode using C99 math.h ceil().
 */

#include <math.h>

case OP_CEIL: {
    Value v = pop_value(vm);
    if (v.type == VAL_INT) {
        /* ceil(n) == n for integers */
        push_value(vm, make_int(v.i));
        free_value(v);
    } else if (v.type == VAL_FLOAT) {
        double r = ceil(v.d);
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
        free_value(v);
    } else {
        fprintf(stderr, "Runtime type error: CEIL expects number, got %s\n", value_type_name(v.type));
        exit(1);
    }
    break;
}
