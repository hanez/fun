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
* @file round.c
 * @brief Implements the OP_ROUND opcode using C99 math.h round().
 *        C99 round() rounds half away from zero.
 */

#include <math.h>

case OP_ROUND: {
    Value v = pop_value(vm);
    if (v.type == VAL_INT) {
        push_value(vm, make_int(v.i));
        free_value(v);
    } else if (v.type == VAL_FLOAT) {
        double r = round(v.d);
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
        fprintf(stderr, "Runtime type error: ROUND expects number, got %s\n", value_type_name(v.type));
        exit(1);
    }
    break;
}
