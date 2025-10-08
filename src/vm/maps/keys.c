/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file keys.c
 * @brief Implements the OP_KEYS opcode for retrieving map keys in the VM.
 *
 * This file handles the OP_KEYS instruction, which retrieves the keys of a map
 * and pushes them as an array onto the stack.
 *
 * Behavior:
 * - Pops the map from the stack.
 * - Retrieves the keys of the map.
 * - Pushes the keys as an array onto the stack.
 *
 * Error Handling:
 * - Exits with an error if the map is of the wrong type.
 *
 * Example:
 * // Bytecode: OP_KEYS
 * // Stack before: [{"a": 1, "b": 2}]
 * // Stack after: [["a", "b"]]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_KEYS: {
    Value m = pop_value(vm);
    if (m.type != VAL_MAP) { fprintf(stderr, "KEYS expects map\n"); exit(1); }
    Value arr = map_keys_array(&m);
    free_value(m);
    push_value(vm, arr);
    break;
}
