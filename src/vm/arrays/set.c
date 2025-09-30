/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file arr_set.c
 * @brief Implements the OP_ARR_SET opcode for setting elements in arrays in the VM.
 *
 * This file handles the OP_ARR_SET instruction, which sets a value at a specified index in an array.
 * The value, index, and array are popped from the stack, and the value is pushed back onto the stack.
 *
 * Behavior:
 * - Pops the value, index, and array from the stack.
 * - Sets the value at the specified index in the array.
 * - Pushes the value back onto the stack.
 *
 * Error Handling:
 * - Exits with an error if the array or index is of the wrong type.
 * - Exits with an error if the index is out of bounds.
 *
 * Example:
 * // Bytecode: OP_ARR_SET
 * // Stack before: [42, 1, [10, 20, 30]]
 * // Stack after: [42]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_SET: {
    Value v = pop_value(vm);
    Value idx = pop_value(vm);
    Value arr = pop_value(vm);
    if (arr.type != VAL_ARRAY || idx.type != VAL_INT) {
        fprintf(stderr, "Runtime type error: ARR_SET expects (array, int, value)\n");
        exit(1);
    }
    if (!array_set(&arr, (int)idx.i, v)) {
        fprintf(stderr, "Runtime error: set index out of range\n");
        exit(1);
    }
    free_value(arr);
    free_value(idx);
    /* v already owned by array; push copy for return value */
    push_value(vm, copy_value(&v));
    free_value(v);
    break;
}
