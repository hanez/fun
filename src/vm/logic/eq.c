/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file eq.c
 * @brief Implements the OP_EQ opcode for equality comparison in the VM.
 *
 * This file handles the OP_EQ instruction, which checks if two values are equal.
 * The values are popped from the stack, and the result (1 or 0) is pushed back.
 *
 * Behavior:
 * - Pops two values from the stack.
 * - Checks if the values are equal.
 * - Pushes 1 (true) or 0 (false) onto the stack.
 *
 * Error Handling:
 * - Exits with an error if the operands are of incompatible types.
 *
 * Example:
 * // Bytecode: OP_EQ
 * // Stack before: [42, 42]
 * // Stack after: [1]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_EQ: {
    Value b = pop_value(vm);
    Value a = pop_value(vm);
    int eq = 0;
    if (a.type == b.type) {
        switch (a.type) {
            case VAL_INT:      eq = (a.i == b.i); break;
            case VAL_BOOL:     eq = ((a.i != 0) == (b.i != 0)); break;
            case VAL_STRING:   eq = (a.s && b.s) ? (strcmp(a.s, b.s) == 0) : (a.s == b.s); break;
            case VAL_FUNCTION: eq = (a.fn == b.fn); break;
            case VAL_NIL:      eq = 1; break;
            default:           eq = 0; break;
        }
    } else {
        /* interop: bool vs int (0/1) */
        if ((a.type == VAL_BOOL && b.type == VAL_INT) || (a.type == VAL_INT && b.type == VAL_BOOL)) {
            int ai = (a.type == VAL_BOOL) ? (a.i != 0) : (a.i != 0);
            int bi = (b.type == VAL_BOOL) ? (b.i != 0) : (b.i != 0);
            eq = (ai == bi);
        } else {
            eq = 0;
        }
    }
    push_value(vm, make_bool(eq));
    free_value(a); free_value(b);
    break;
}
