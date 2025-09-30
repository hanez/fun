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
        Value out = make_int(v.i);
        free_value(v);
        push_value(vm, out);
    } else if (v.type == VAL_STRING) {
        const char *s = v.s ? v.s : "";
        const char *p = s;
        while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') p++;
        char *endp = NULL;
        long long parsed = strtoll(p, &endp, 10);
        while (endp && (*endp == ' ' || *endp == '\t' || *endp == '\r' || *endp == '\n')) endp++;
        if (endp && *endp != '\0') {
            push_value(vm, make_int(0));
        } else {
            push_value(vm, make_int((int64_t)parsed));
        }
        free_value(v);
    } else {
        free_value(v);
        push_value(vm, make_int(0));
    }
    break;
}
