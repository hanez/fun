/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file contains.c
 * @brief Implements the OP_CONTAINS opcode for checking array membership in the VM.
 *
 * This file handles the OP_CONTAINS instruction, which checks if a value is present in an array.
 * The value and array are popped from the stack, and a boolean result (1/0) is pushed back.
 *
 * Behavior:
 * - Pops the value and array from the stack.
 * - Checks if the value is present in the array.
 * - Pushes 1 (true) or 0 (false) onto the stack.
 *
 * Error Handling:
 * - Exits with an error if the array is of the wrong type.
 *
 * Example:
 * // Bytecode: OP_CONTAINS
 * // Stack before: [20, [10, 20, 30]]
 * // Stack after: [1]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_CONTAINS: {
    Value needle = pop_value(vm);
    Value arr = pop_value(vm);
    if (arr.type != VAL_ARRAY) {
        fprintf(stderr, "Runtime type error: CONTAINS expects (array, value)\n");
        exit(1);
    }
    int ok = array_contains(&arr, &needle);
    free_value(arr);
    free_value(needle);
    push_value(vm, make_int(ok ? 1 : 0));
    break;
}
