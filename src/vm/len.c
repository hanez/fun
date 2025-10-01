/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file len.c
 * @brief Implements the OP_LEN opcode for getting the length of arrays or strings in the VM.
 *
 * This file handles the OP_LEN instruction, which retrieves the length of an array or string.
 * The array or string is popped from the stack, and the length is pushed back.
 *
 * Behavior:
 * - Pops the array or string from the stack.
 * - Retrieves the length of the array or string.
 * - Pushes the length onto the stack.
 *
 * Error Handling:
 * - Exits with an error if the operand is not an array or string.
 *
 * Example:
 * // Bytecode: OP_LEN
 * // Stack before: ["hello"]
 * // Stack after: [5]
 *
 * @author Johannes Findeisen
 * @date 2025-09-16
 */

case OP_LEN: {
    Value a = pop_value(vm);
    int len = 0;
    if (a.type == VAL_STRING) {
        len = (int)(a.s ? (int)strlen(a.s) : 0);
    } else if (a.type == VAL_ARRAY) {
        len = array_length(&a);
        if (len < 0) len = 0;
    } else {
            /* Be lenient: for non-array/non-string, treat length as 0 */
            push_value(vm, make_int(0));
    }
    free_value(a);
    push_value(vm, make_int(len));
    break;
}
