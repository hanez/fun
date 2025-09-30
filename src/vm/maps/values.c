/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file values.c
 * @brief Implements the OP_VALUES opcode for retrieving map values in the VM.
 *
 * This file handles the OP_VALUES instruction, which retrieves the values of a map
 * and pushes them as an array onto the stack.
 *
 * Behavior:
 * - Pops the map from the stack.
 * - Retrieves the values of the map.
 * - Pushes the values as an array onto the stack.
 *
 * Error Handling:
 * - Exits with an error if the map is of the wrong type.
 *
 * Example:
 * // Bytecode: OP_VALUES
 * // Stack before: [{"a": 1, "b": 2}]
 * // Stack after: [[1, 2]]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_VALUES: {
    Value m = pop_value(vm);
    if (m.type != VAL_MAP) { fprintf(stderr, "VALUES expects map\n"); exit(1); }
    Value arr = map_values_array(&m);
    free_value(m);
    push_value(vm, arr);
    break;
}
