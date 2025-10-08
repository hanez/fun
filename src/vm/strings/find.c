/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file find.c
 * @brief Implements the OP_FIND opcode for finding substrings in the VM.
 *
 * This file handles the OP_FIND instruction, which finds the index of a substring
 * within a string. The substring and string are popped from the stack, and the index
 * (or -1) is pushed back.
 *
 * Behavior:
 * - Pops the substring and string from the stack.
 * - Finds the index of the substring within the string.
 * - Pushes the index (or -1 if not found) onto the stack.
 *
 * Error Handling:
 * - Exits with an error if the operands are not strings.
 *
 * Example:
 * // Bytecode: OP_FIND
 * // Stack before: ["world", "hello world"]
 * // Stack after: [6]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_FIND: {
    Value needle = pop_value(vm);
    Value hay = pop_value(vm);
    if (hay.type != VAL_STRING || needle.type != VAL_STRING) {
        fprintf(stderr, "Runtime type error: FIND expects (string, string)\n");
        exit(1);
    }
    int idx = bi_find(&hay, &needle);
    free_value(hay);
    free_value(needle);
    push_value(vm, make_int(idx));
    break;
}
