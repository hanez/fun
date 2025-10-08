/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file make_array.c
 * @brief Implements the OP_MAKE_ARRAY opcode for creating arrays in the VM.
 *
 * This file handles the OP_MAKE_ARRAY instruction, which pops `n` values from the stack,
 * creates an array from them, and pushes the resulting array back onto the stack.
 *
 * Behavior:
 * - Validates the number of elements (`n`) to ensure it is non-negative and within stack bounds.
 * - Allocates temporary storage for the array elements.
 * - Constructs the array using `make_array_from_values`.
 * - Frees temporary storage and pushes the array onto the stack.
 *
 * Error Handling:
 * - Exits with an error if `n` is invalid or if memory allocation fails.
 *
 * Example:
 * // Bytecode: OP_MAKE_ARRAY 3
 * // Stack before: [1, 2, 3]
 * // Stack after: [[1, 2, 3]]
 *
 * @author Johanes Findeisen
 * @date 2025-10-16
 */

case OP_MAKE_ARRAY: {
    int n = inst.operand;
    if (n < 0 || vm->sp + 1 < n) {
        fprintf(stderr, "Runtime error: invalid element count for MAKE_ARRAY\n");
        exit(1);
    }
    /* pop n values into temp array preserving original order */
    Value *vals = (Value*)malloc(sizeof(Value) * n);
    if (!vals) { fprintf(stderr, "Runtime error: OOM in MAKE_ARRAY\n"); exit(1); }
    for (int i = n - 1; i >= 0; --i) {
        vals[i] = pop_value(vm); /* take ownership */
    }
    /* build array by copying values, then free originals */
    Value arr = make_array_from_values(vals, n);
    for (int i = 0; i < n; ++i) {
        free_value(vals[i]);
    }
    free(vals);
    push_value(vm, arr);
    break;
}
