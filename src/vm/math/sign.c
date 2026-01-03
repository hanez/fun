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
* @file sign.c
 * @brief Implements the OP_SIGN opcode returning -1, 0, or 1.
 */

case OP_SIGN: {
    Value v = pop_value(vm);
    int out = 0;
    if (v.type == VAL_INT) {
        out = (v.i > 0) - (v.i < 0);
    } else if (v.type == VAL_FLOAT) {
        if (v.d > 0.0) out = 1;
        else if (v.d < 0.0) out = -1;
        else out = 0;
    } else {
        fprintf(stderr, "Runtime type error: SIGN expects number, got %s\n", value_type_name(v.type));
        exit(1);
    }
    free_value(v);
    push_value(vm, make_int(out));
    break;
}
