/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file abs.c
 * @brief Implements the OP_ABS opcode for absolute value in the VM.
 *
 * This file handles the OP_ABS instruction, which computes the absolute value
 * of an integer.
 *
 * Behavior:
 * - Pops value from stack
 * - Pushes |value|
 *
 * Error Handling:
 * - Exits if not integer
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_ABS: {
    Value x = pop_value(vm);
    if (x.type != VAL_INT) { fprintf(stderr, "ABS expects int\n"); exit(1); }
    int64_t v = x.i;
    if (v < 0) v = -v;
    push_value(vm, make_int(v));
    free_value(x);
    break;
}
