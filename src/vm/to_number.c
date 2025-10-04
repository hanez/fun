/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file to_number.c
 * @brief Implements the OP_TO_NUMBER opcode for converting values to integers in the VM.
 *
 * This file handles the OP_TO_NUMBER instruction, which converts a value to an integer.
 * The value is popped from the stack, and the result is pushed back.
 *
 * Behavior:
 * - Pops the value from the stack.
 * - Converts the value to an integer.
 * - Pushes the result onto the stack.
 *
 * Error Handling:
 * - Exits with an error if the conversion fails.
 *
 * Example:
 * // Bytecode: OP_TO_NUMBER
 * // Stack before: ["42"]
 * // Stack after: [42]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_TO_NUMBER: {
    Value v = pop_value(vm);
    if (v.type == VAL_INT) {
        push_value(vm, make_int(v.i));
        free_value(v);
    } else if (v.type == VAL_FLOAT) {
        double d = v.d;
        if (d >= (double)INT64_MIN && d <= (double)INT64_MAX) {
            int64_t ii = (int64_t)d;
            if ((double)ii == d) {
                push_value(vm, make_int(ii));
            } else {
                push_value(vm, make_float(d));
            }
        } else {
            push_value(vm, make_float(d));
        }
        free_value(v);
    } else if (v.type == VAL_STRING) {
        const char *s = v.s ? v.s : "";
        const char *p = s;
        while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') p++;
        char *endp = NULL;
        /* Try float first to support decimals and scientific notation */
        double dval = strtod(p, &endp);
        while (endp && (*endp == ' ' || *endp == '\t' || *endp == '\r' || *endp == '\n')) endp++;
        if (!endp || *endp != '\0') {
            /* Fallback to integer-only parse */
            endp = NULL;
            long long parsed = strtoll(p, &endp, 10);
            while (endp && (*endp == ' ' || *endp == '\t' || *endp == '\r' || *endp == '\n')) endp++;
            if (endp && *endp == '\0') {
                push_value(vm, make_int((int64_t)parsed));
            } else {
                push_value(vm, make_int(0));
            }
        } else {
            /* Preserve int when exact; else float */
            if (dval >= (double)INT64_MIN && dval <= (double)INT64_MAX) {
                int64_t ii = (int64_t)dval;
                if ((double)ii == dval) {
                    push_value(vm, make_int(ii));
                } else {
                    push_value(vm, make_float(dval));
                }
            } else {
                push_value(vm, make_float(dval));
            }
        }
        free_value(v);
    } else if (v.type == VAL_BOOL) {
        push_value(vm, make_int(v.i ? 1 : 0));
        free_value(v);
    } else {
        free_value(v);
        push_value(vm, make_int(0));
    }
    break;
}
