/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the ISC license.
 * https://opensource.org/license/isc-license-txt
 */

/**
* @file index_get.c
 * @brief Implements the OP_INDEX_GET opcode for array and map indexing in the VM.
 *
 * This file handles the OP_INDEX_GET instruction, which retrieves an element from
 * an array or a value from a map using an index or key.
 *
 * Behavior:
 * - Pops the index/key and container from the stack.
 * - For arrays, retrieves the element at the specified index.
 * - For maps, retrieves the value associated with the specified key.
 * - Pushes the retrieved value onto the stack.
 *
 * Error Handling:
 * - Exits with an error if the container is not an array or map, if the index/key
 *   is of the wrong type, or if the index is out of bounds.
 *
 * Example:
 * // Bytecode: OP_INDEX_GET
 * // Stack before: [1, [10, 20, 30]]
 * // Stack after: [20]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_INDEX_GET: {
    Value idx = pop_value(vm);
    Value container = pop_value(vm);
#ifdef FUN_DEBUG
    fprintf(stderr, "DEBUG INDEX_GET: container.type=%d idx.type=%d\n",
            container.type, idx.type);
#endif
    if (container.type == VAL_ARRAY) {
        if (idx.type != VAL_INT) { fprintf(stderr, "INDEX_GET index must be int for array\n"); exit(1); }
        Value elem;
        if (!array_get_copy(&container, (int)idx.i, &elem)) {
            fprintf(stderr, "Runtime error: index out of range\n"); exit(1);
        }
        free_value(container);
        free_value(idx);
        push_value(vm, elem);
    } else if (container.type == VAL_MAP) {
        if (idx.type != VAL_STRING) { fprintf(stderr, "INDEX_GET key must be string for map\n"); exit(1); }
        Value out;
        if (!map_get_copy(&container, idx.s ? idx.s : "", &out)) {
            out = make_nil();
        }
        free_value(container);
        free_value(idx);
        push_value(vm, out);
    } else {
        fprintf(stderr, "Runtime type error: INDEX_GET expects array or map (got container=%s, index=%s)\n",
                value_type_name(container.type), value_type_name(idx.type));
        exit(1);
    }
    break;
}
