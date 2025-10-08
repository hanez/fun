/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file random_int.c
 * @brief Implements the OP_RANDOM_INT opcode for generating random integers in the VM.
 *
 * This file handles the OP_RANDOM_INT instruction, which generates a random integer
 * within a specified range. The range bounds are popped from the stack, and the result
 * is pushed back.
 *
 * Behavior:
 * - Pops the upper and lower bounds from the stack.
 * - Generates a random integer within the range.
 * - Pushes the result onto the stack.
 *
 * Error Handling:
 * - Exits with an error if the bounds are not integers.
 * - Exits with an error if the lower bound is greater than the upper bound.
 *
 * Example:
 * // Bytecode: OP_RANDOM_INT
 * // Stack before: [10, 1]
 * // Stack after: [7]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_RANDOM_INT: {
    Value hi = pop_value(vm);
    Value lo = pop_value(vm);
    if (lo.type != VAL_INT || hi.type != VAL_INT) { fprintf(stderr, "RANDOM_INT expects (int, int)\n"); exit(1); }
    int64_t a = lo.i, b = hi.i;
    if (b <= a) { push_value(vm, make_int((int64_t)a)); free_value(lo); free_value(hi); break; }
    int64_t span = b - a;
    int64_t r = (int64_t)(rand() % (span));
    push_value(vm, make_int(a + r));
    free_value(lo); free_value(hi);
    break;
}
