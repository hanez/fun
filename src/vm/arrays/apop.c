/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file apop.c
 * @brief Implements the OP_APOP opcode for removing elements from arrays in the VM.
 *
 * This file handles the OP_APOP instruction, which removes the last element from an array
 * and pushes the removed element onto the stack. The array is popped from the stack.
 *
 * Behavior:
 * - Pops the array from the stack.
 * - Removes the last element from the array.
 * - Pushes the removed element onto the stack.
 *
 * Error Handling:
 * - Exits with an error if the array is of the wrong type.
 * - Exits with an error if the array is empty.
 *
 * Example:
 * // Bytecode: OP_APOP
 * // Stack before: [[10, 20, 30]]
 * // Stack after: [30]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_APOP: {
    Value arr = pop_value(vm);
    if (arr.type != VAL_ARRAY) {
        fprintf(stderr, "Runtime type error: ARR_APOP expects array\n");
        exit(1);
    }
    Value out;
    if (!array_pop(&arr, &out)) {
        fprintf(stderr, "Runtime error: pop from empty array\n");
        exit(1);
    }
    free_value(arr);
    push_value(vm, out);
    break;
}
