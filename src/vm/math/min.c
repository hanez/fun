/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file min.c
 * @brief Implements the OP_MIN opcode for finding the minimum of two values in the VM.
 *
 * This file handles the OP_MIN instruction, which finds the minimum of two integer values.
 * The values are popped from the stack, and the result is pushed back.
 *
 * Behavior:
 * - Pops two integer values from the stack.
 * - Finds the minimum of the two values.
 * - Pushes the result onto the stack.
 *
 * Error Handling:
 * - Exits with an error if the operands are not integers.
 *
 * Example:
 * // Bytecode: OP_MIN
 * // Stack before: [10, 42]
 * // Stack after: [10]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_MIN: {
    Value b = pop_value(vm);
    Value a = pop_value(vm);
    if (a.type != VAL_INT || b.type != VAL_INT) { fprintf(stderr, "MIN expects ints\n"); exit(1); }
    push_value(vm, make_int(a.i < b.i ? a.i : b.i));
    free_value(a); free_value(b);
    break;
}
