/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file lte.c
 * @brief Implements the OP_LTE opcode for less-than-or-equal comparison in the VM.
 *
 * This file handles the OP_LTE instruction, which checks if the first value is less than or equal to the second.
 * The values are popped from the stack, and the result (1 or 0) is pushed back.
 *
 * Behavior:
 * - Pops two values from the stack.
 * - Checks if the first value is less than or equal to the second.
 * - Pushes 1 (true) or 0 (false) onto the stack.
 *
 * Error Handling:
 * - Exits with an error if the operands are of incompatible types.
 *
 * Example:
 * // Bytecode: OP_LTE
 * // Stack before: [42, 42]
 * // Stack after: [1]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_LTE: {
    Value b = pop_value(vm);
    Value a = pop_value(vm);
    if (a.type != VAL_INT || b.type != VAL_INT) {
        fprintf(stderr, "Runtime type error: LTE expects ints\n");
        exit(1);
    }
    Value res = make_int(a.i <= b.i ? 1 : 0);
    free_value(a);
    free_value(b);
    push_value(vm, res);
    break;
}
