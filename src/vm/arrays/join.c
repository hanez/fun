/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the ISC license.
 * https://opensource.org/license/isc-license-txt
 */

/**
* @file join.c
 * @brief Implements the OP_JOIN opcode for joining array elements into a string in the VM.
 *
 * This file handles the OP_JOIN instruction, which joins the elements of an array into a string
 * using a separator. The separator and array are popped from the stack, and the resulting string
 * is pushed back.
 *
 * Behavior:
 * - Pops the separator and array from the stack.
 * - Joins the array elements into a string using the separator.
 * - Pushes the resulting string onto the stack.
 *
 * Error Handling:
 * - Exits with an error if the array or separator is of the wrong type.
 *
 * Example:
 * // Bytecode: OP_JOIN
 * // Stack before: [", ", ["a", "b", "c"]]
 * // Stack after: ["a, b, c"]
 * 
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_JOIN: {
    Value sep = pop_value(vm);
    Value arr = pop_value(vm);
    if (arr.type != VAL_ARRAY || sep.type != VAL_STRING) {
        fprintf(stderr, "Runtime type error: JOIN expects (array, string)\n");
        exit(1);
    }
    Value out = bi_join(&arr, &sep);
    free_value(arr);
    free_value(sep);
    push_value(vm, out);
    break;
}
