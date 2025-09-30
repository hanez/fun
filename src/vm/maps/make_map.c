/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file make_map.c
 * @brief Implements the OP_MAKE_MAP opcode for creating maps in the VM.
 *
 * This file handles the OP_MAKE_MAP instruction, which pops `pairs` key-value pairs
 * from the stack, creates a map from them, and pushes the resulting map back onto the stack.
 *
 * Behavior:
 * - Validates the number of pairs to ensure it is non-negative.
 * - Ensures that map keys are strings.
 * - Constructs the map using `make_map_empty` and `map_set`.
 * - Pushes the map onto the stack.
 *
 * Error Handling:
 * - Exits with an error if the number of pairs is invalid, if keys are not strings,
 *   or if map construction fails.
 *
 * Example:
 * // Bytecode: OP_MAKE_MAP 2
 * // Stack before: ["key1", 1, "key2", 2]
 * // Stack after: [{"key1": 1, "key2": 2}]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */


case OP_MAKE_MAP: {
    int pairs = inst.operand;
    if (pairs < 0) { fprintf(stderr, "MAKE_MAP invalid pair count\n"); exit(1); }
    Value m = make_map_empty();
    for (int i = 0; i < pairs; ++i) {
        Value val = pop_value(vm);
        Value key = pop_value(vm);
        if (key.type != VAL_STRING) { fprintf(stderr, "Map literal keys must be strings\n"); exit(1); }
        if (!map_set(&m, key.s ? key.s : "", val)) {
            fprintf(stderr, "Map literal set failed\n"); exit(1);
        }
        free_value(key);
    }
    push_value(vm, m);
    break;
}
