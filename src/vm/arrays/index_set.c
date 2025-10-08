/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file index_set.c
 * @brief Implements the OP_INDEX_SET opcode for array and map assignment in the VM.
 *
 * This file handles the OP_INDEX_SET instruction, which assigns a value to an
 * element in an array or a key in a map.
 *
 * Behavior:
 * - Pops the value, index/key, and container from the stack.
 * - For arrays, assigns the value to the specified index.
 * - For maps, assigns the value to the specified key.
 *
 * Error Handling:
 * - Exits with an error if the container is not an array or map, if the index/key
 *   is of the wrong type, or if the index is out of bounds.
 *
 * Example:
 * // Bytecode: OP_INDEX_SET
 * // Stack before: [42, 1, [10, 20, 30]]
 * // Stack after: [[10, 42, 30]]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */


case OP_INDEX_SET: {
    Value v = pop_value(vm);
    Value idx = pop_value(vm);
    Value container = pop_value(vm);
#ifdef FUN_DEBUG
    fprintf(stderr, "DEBUG INDEX_SET: container.type=%d idx.type=%d value.type=%d\n",
            container.type, idx.type, v.type);
#endif
    if (container.type == VAL_ARRAY) {
        if (idx.type != VAL_INT) { fprintf(stderr, "INDEX_SET index must be int for array\n"); exit(1); }
        if (!array_set(&container, (int)idx.i, v)) {
            fprintf(stderr, "Runtime error: index out of range\n"); exit(1);
        }
        free_value(container);
        free_value(idx);
    } else if (container.type == VAL_MAP) {
        if (idx.type != VAL_STRING) { fprintf(stderr, "INDEX_SET key must be string for map\n"); exit(1); }
        if (!map_set(&container, idx.s ? idx.s : "", v)) {
            fprintf(stderr, "Runtime error: map set failed\n"); exit(1);
        }
        free_value(container);
        free_value(idx);
    } else {
        fprintf(stderr, "Runtime type error: INDEX_SET expects array or map\n");
        exit(1);
    }
    break;
}
