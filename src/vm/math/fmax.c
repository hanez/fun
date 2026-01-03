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
* @file fmax.c
 * @brief Implements the OP_FMAX opcode using C99 math.h fmax().
 *        Accepts int or float; follows IEEE-754 NaN handling per fmax.
 */

#include <math.h>

case OP_FMAX: {
    Value b = pop_value(vm);
    Value a = pop_value(vm);
    if (!((a.type == VAL_INT || a.type == VAL_FLOAT) && (b.type == VAL_INT || b.type == VAL_FLOAT))) {
        fprintf(stderr, "Runtime type error: FMAX expects numbers, got %s and %s\n",
                value_type_name(a.type), value_type_name(b.type));
        exit(1);
    }
    double da = (a.type == VAL_FLOAT) ? a.d : (double)a.i;
    double db = (b.type == VAL_FLOAT) ? b.d : (double)b.i;
    double r = fmax(da, db);
    Value out;
    if (!isnan(r) && !isinf(r) && r >= (double)INT64_MIN && r <= (double)INT64_MAX) {
        int64_t ii = (int64_t)r;
        if ((double)ii == r) out = make_int(ii); else out = make_float(r);
    } else {
        out = make_float(r);
    }
    free_value(a); free_value(b);
    push_value(vm, out);
    break;
}
