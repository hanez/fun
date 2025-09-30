/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file clear.c
 * @brief Implements the OP_CLEAR opcode for clearing arrays in the VM.
 *
 * This file handles the OP_CLEAR instruction, which clears all elements from an array.
 * The array is popped from the stack, and nothing is pushed back.
 *
 * Behavior:
 * - Pops the array from the stack.
 * - Clears all elements from the array.
 *
 * Error Handling:
 * - Exits with an error if the array is of the wrong type.
 *
 * Example:

 * // Bytecode: OP_CLEAR
 * // Stack before: [[10, 20, 30]]
 * // Stack after: []
 *
 * @author Johannes Findeise
 * @date 2025-10-16
 */

case OP_CLEAR: {
    Value arr = pop_value(vm);
    if (arr.type != VAL_ARRAY) {
        fprintf(stderr, "Runtime type error: CLEAR expects array\n");
        exit(1);
    }
    array_clear(&arr);
    free_value(arr);
    push_value(vm, make_int(0));
    break;
}
