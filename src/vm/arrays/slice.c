/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file slice.c
 * @brief Implements the OP_SLICE opcode for array slicing in the VM.
 *
 * This file handles the OP_SLICE instruction, which creates a new array containing
 * elements from the original array between specified start and end indices.
 *
 * Behavior:
 * - Pops end index, start index, and array from the stack
 * - Creates a new array containing elements from start to end-1
 * - Pushes the new array onto the stack
 *
 * Error Handling:
 * - Exits with error if arguments are wrong types
 * - Handles negative indices and out-of-bounds cases gracefully
 *
 * Example:
 * // Bytecode: OP_SLICE
 * // Stack before: [3, 1, [10,20,30,40]]
 * // Stack after: [[20,30]]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_SLICE: {
    Value end = pop_value(vm);
    Value start = pop_value(vm);
    Value arr = pop_value(vm);
    if (arr.type != VAL_ARRAY || start.type != VAL_INT || end.type != VAL_INT) {
        fprintf(stderr, "Runtime type error: SLICE expects (array, int, int)\n");
        exit(1);
    }
    Value out = array_slice(&arr, (int)start.i, (int)end.i);
    free_value(arr);
    free_value(start);
    free_value(end);
    push_value(vm, out);
    break;
}
