/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file neq.c
 * @brief Implements the OP_NEQ opcode for inequality comparison in the VM.
 *
 * This file handles the OP_NEQ instruction, which checks if two values are not equal.
 * The values are popped from the stack, and the result (1 or 0) is pushed back.
 *
 * Behavior:
 * - Pops two values from the stack.
 * - Checks if the values are not equal.
 * - Pushes 1 (true) or 0 (false) onto the stack.
 *
 * Error Handling:
 * - Exits with an error if the operands are of incompatible types.
 *
 * Example:
 * // Bytecode: OP_NEQ
 * // Stack before: [42, 10]
 * // Stack after: [1]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_NEQ: {
    Value b = pop_value(vm);
    Value a = pop_value(vm);
    int neq = 1;
    if (a.type == b.type) {
        switch (a.type) {
            case VAL_INT:      neq = (a.i != b.i); break;
            case VAL_BOOL:     neq = ((a.i != 0) != (b.i != 0)); break;
            case VAL_STRING:   neq = (a.s && b.s) ? (strcmp(a.s, b.s) != 0) : (a.s != b.s); break;
            case VAL_FUNCTION: neq = (a.fn != b.fn); break;
            case VAL_NIL:      neq = 0; break;
            default:           neq = 1; break;
        }
    } else {
        /* interop: bool vs int (0/1) */
        if ((a.type == VAL_BOOL && b.type == VAL_INT) || (a.type == VAL_INT && b.type == VAL_BOOL)) {
            int ai = (a.type == VAL_BOOL) ? (a.i != 0) : (a.i != 0);
            int bi = (b.type == VAL_BOOL) ? (b.i != 0) : (b.i != 0);
            neq = (ai != bi);
        } else {
            neq = 1;
        }
    }
    push_value(vm, make_bool(neq));
    free_value(a); free_value(b);
    break;
}
