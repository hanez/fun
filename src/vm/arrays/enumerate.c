/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file enumerate.c
 * @brief Implements the OP_ENUMERATE opcode for enumerating arrays in the VM.
 *
 * This file handles the OP_ENUMERATE instruction, which creates an array of [index, value] pairs
 * from an array. The array is popped from the stack, and the new array is pushed back.
 *
 * Behavior:
 * - Pops the array from the stack.
 * - Creates a new array of [index, value] pairs.
 * - Pushes the new array onto the stack.
 *
 * Error Handling:
 * - Exits with an error if the array is of the wrong type.
 *
 * Example:
 * // Bytecode: OP_ENUMERATE
 * // Stack before: [[10, 20, 30]]
 * // Stack after: [[[0, 10], [1, 20], [2, 30]]]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_ENUMERATE: {
    Value arr = pop_value(vm);
    if (arr.type != VAL_ARRAY) {
        fprintf(stderr, "Runtime type error: ENUMERATE expects array\n");
        exit(1);
    }
    Value out = bi_enumerate(&arr);
    free_value(arr);
    push_value(vm, out);
    break;
}
