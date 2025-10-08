/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file gte.c
 * @brief Implements the OP_GTE opcode for greater-than-or-equal comparison in the VM.
 *
 * This file handles the OP_GTE instruction, which checks if the first value is greater than or equal to the second.
 * The values are popped from the stack, and the result (1 or 0) is pushed back.
 *
 * Behavior:
 * - Pops two values from the stack.
 * - Checks if the first value is greater than or equal to the second.
 * - Pushes 1 (true) or 0 (false) onto the stack.
 *
 * Error Handling:
 * - Exits with an error if the operands are of incompatible types.
 *
 * Example:
 * // Bytecode: OP_GTE
 * // Stack before: [42, 42]
 * // Stack after: [1]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_GTE: {
    Value b = pop_value(vm);
    Value a = pop_value(vm);
    if (a.type != VAL_INT || b.type != VAL_INT) {
        fprintf(stderr, "Runtime type error: GTE expects ints\n");
        exit(1);
    }
    push_value(vm, make_int(a.i >= b.i ? 1 : 0));
    free_value(a); free_value(b);
    break;
}
