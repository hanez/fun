/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file random_seed.c
 * @brief Implements the OP_RANDOM_SEED opcode for seeding the random number generator in the VM.
 *
 * This file handles the OP_RANDOM_SEED instruction, which seeds the random number generator
 * with a specified value. The seed is popped from the stack.
 *
 * Behavior:
 * - Pops the seed value from the stack.
 * - Seeds the random number generator with the value.
 *
 * Error Handling:
 * - Exits with an error if the seed is not an integer.
 *
 * Example:
 * // Bytecode: OP_RANDOM_SEED
 * // Stack before: [42]
 * // Stack after: []
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_RANDOM_SEED: {
    Value seed = pop_value(vm);
    if (seed.type != VAL_INT) { fprintf(stderr, "RANDOM_SEED expects int\n"); exit(1); }
    srand((unsigned int)seed.i);
    free_value(seed);
    push_value(vm, make_int(0));
    break;
}
