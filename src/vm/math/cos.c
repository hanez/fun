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
* @file cos.c
 * @brief Implements the OP_COS opcode using C99 math.h cos().
 */

#include <math.h>

case OP_COS: {
    Value v = pop_value(vm);
    if (v.type == VAL_INT || v.type == VAL_FLOAT) {
        double x = (v.type == VAL_FLOAT) ? v.d : (double)v.i;
        double r = cos(x);
        push_value(vm, make_float(r));
        free_value(v);
    } else {
        fprintf(stderr, "Runtime type error: COS expects number, got %s\n", value_type_name(v.type));
        exit(1);
    }
    break;
}
