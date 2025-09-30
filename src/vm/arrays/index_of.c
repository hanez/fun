/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file index_of.c
 * @brief Implements the OP_INDEX_OF opcode for finding the index of a value in an array in the VM.
 *
 * This file handles the OP_INDEX_OF instruction, which finds the index of a value in an array.
 * The value and array are popped from the stack, and the index (or -1) is pushed back.
 *
 * Behavior:
 * - Pops the value and array from the stack.
 * - Finds the index of the value in the array.
 * - Pushes the index (or -1 if not found) onto the stack.
 *
 * Error Handling:
 * - Exits with an error if the array is of the wrong type.
 *
 * Example:
 * // Bytecode: OP_INDEX_OF
 * // Stack before: [20, [10, 20, 30]]
 * // Stack after: [1]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_INDEX_OF: {
    Value needle = pop_value(vm);
    Value arr = pop_value(vm);
    if (arr.type != VAL_ARRAY) {
        fprintf(stderr, "Runtime type error: INDEX_OF expects (array, value)\n");
        exit(1);
    }
    int idx = array_index_of(&arr, &needle);
    free_value(arr);
    free_value(needle);
    push_value(vm, make_int(idx));
    break;
}
