/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file split.c
 * @brief Implements the OP_SPLIT opcode for splitting strings in the VM.
 *
 * This file handles the OP_SPLIT instruction, which splits a string into an array
 * of substrings using a separator. The separator and string are popped from the stack,
 * and the resulting array is pushed back.
 *
 * Behavior:
 * - Pops the separator and string from the stack.
 * - Splits the string into an array of substrings using the separator.
 * - Pushes the resulting array onto the stack.
 *
 * Error Handling:
 * - Exits with an error if the operands are not strings.
 *
 * Example:
 * // Bytecode: OP_SPLIT
 * // Stack before: [", ", "a, b, c"]
 * // Stack after: [["a", "b", "c"]]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_SPLIT: {
    Value sep = pop_value(vm);
    Value str = pop_value(vm);
    if (str.type != VAL_STRING || sep.type != VAL_STRING) {
        fprintf(stderr, "Runtime type error: SPLIT expects (string, string)\n");
        exit(1);
    }
    Value out = bi_split(&str, &sep);
    free_value(str);
    free_value(sep);
    push_value(vm, out);
    break;
}
