/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file div.c
 * @brief Implements the OP_DIV opcode for integer division in the VM.
 *
 * This file handles the OP_DIV instruction, which performs integer division
 * on two integer values popped from the stack and pushes the result back onto the stack.
 *
 * Behavior:
 * - Pops two integer values from the stack.
 * - Performs integer division (`a / b`).
 * - Pushes the result back onto the stack.
 *
 * Error Handling:
 * - Exits with an error if the operands are not integers.
 * - Exits with an error if division by zero is attempted.
 *
 * Example:
 * // Bytecode: OP_DIV
 * // Stack before: [10, 2]
 * // Stack after: [5]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_DIV: {
    Value b = pop_value(vm);
    Value a = pop_value(vm);
    if ((a.type == VAL_INT || a.type == VAL_FLOAT) && (b.type == VAL_INT || b.type == VAL_FLOAT)) {
        if (a.type == VAL_FLOAT || b.type == VAL_FLOAT) {
            double da = (a.type == VAL_FLOAT) ? a.d : (double)a.i;
            double db = (b.type == VAL_FLOAT) ? b.d : (double)b.i;
            if (db == 0.0) {
                fprintf(stderr, "Runtime error: division by zero\n");
                exit(1);
            }
            Value res = make_float(da / db);
            free_value(a);
            free_value(b);
            push_value(vm, res);
        } else {
            if (b.i == 0) {
                fprintf(stderr, "Runtime error: division by zero\n");
                exit(1);
            }
            Value res = make_int(a.i / b.i);
            free_value(a);
            free_value(b);
            push_value(vm, res);
        }
    } else {
        fprintf(stderr, "Runtime type error: DIV expects numbers, got %s and %s\n",
                value_type_name(a.type), value_type_name(b.type));
        exit(1);
    }
    break;
}
