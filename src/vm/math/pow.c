/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the ISC license.
 * https://opensource.org/license/isc-license-txt
 */

/**
* @file pow.c
 * @brief Implements the OP_POW opcode for exponentiation in the VM.
 *
 * This file handles the OP_POW instruction, which computes the power of two integer values.
 * The values are popped from the stack, and the result is pushed back.
 *
 * Behavior:
 * - Pops two integer values from the stack.
 * - Computes the power of the first value raised to the second.
 * - Pushes the result onto the stack.
 *
 * Error Handling:
 * - Exits with an error if the operands are not integers.
 *
 * Example:
 * // Bytecode: OP_POW
 * // Stack before: [2, 3]
 * // Stack after: [8]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_POW: {
    Value b = pop_value(vm);
    Value a = pop_value(vm);
    if (a.type != VAL_INT || b.type != VAL_INT) { fprintf(stderr, "POW expects ints\n"); exit(1); }
    int64_t base = a.i;
    int64_t exp = b.i;
    int64_t res = 1;
    if (exp < 0) { res = 0; } else {
        while (exp > 0) {
            if (exp & 1) res *= base;
            base *= base;
            exp >>= 1;
        }
    }
    push_value(vm, make_int(res));
    free_value(a); free_value(b);
    break;
}
