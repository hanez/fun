/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the ISC license.
 * https://opensource.org/license/isc-license-txt
 */

/**
* @file arr_insert.c
 * @brief Implements the OP_ARR_INSERT opcode for inserting elements into arrays in the VM.
 *
 * This file handles the OP_ARR_INSERT instruction, which inserts a value into an array
 * at a specified index. The array, index, and value are popped from the stack, and the
 * new length of the array is pushed back onto the stack.
 *
 * Behavior:
 * - Pops the value, index, and array from the stack.
 * - Inserts the value into the array at the specified index.
 * - Pushes the new length of the array onto the stack.
 *
 * Error Handling:
 * - Exits with an error if the array or index is of the wrong type.
 * - Exits with an error if memory allocation fails during the insertion.
 *
 * Example:
 * // Bytecode: OP_ARR_INSERT
 * // Stack before: [42, 1, [10, 20, 30]]
 * // Stack after: [4]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_ARR_INSERT: {
    Value v = pop_value(vm);
    Value idx = pop_value(vm);
    Value arr = pop_value(vm);
    if (arr.type != VAL_ARRAY || idx.type != VAL_INT) {
        fprintf(stderr, "Runtime type error: ARR_INSERT expects (array, int, value)\n");
        exit(1);
    }
    int n = array_insert(&arr, (int)idx.i, v);
    if (n < 0) {
        fprintf(stderr, "Runtime error: insert failed (OOM?)\n");
        exit(1);
    }
    free_value(arr);
    free_value(idx);
    push_value(vm, make_int(n));
    break;
}
