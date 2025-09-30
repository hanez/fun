/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file substr.c
 * @brief Implements the OP_SUBSTR opcode for extracting substrings in the VM.
 *
 * This file handles the OP_SUBSTR instruction, which extracts a substring from a string
 * using a start index and length. The length, start index, and string are popped from the stack,
 * and the resulting substring is pushed back.
 *
 * Behavior:
 * - Pops the length, start index, and string from the stack.
 * - Extracts the substring from the string.
 * - Pushes the resulting substring onto the stack.
 *
 * Error Handling:
 * - Exits with an error if the operands are not integers or strings.
 * - Exits with an error if the start index or length is out of bounds.
 *
 * Example:
 * // Bytecode: OP_SUBSTR
 * // Stack before: [5, 6, "hello world"]
 * // Stack after: ["world"]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_SUBSTR: {
    Value lenv = pop_value(vm);
    Value startv = pop_value(vm);
    Value str = pop_value(vm);
    if (str.type != VAL_STRING || startv.type != VAL_INT || lenv.type != VAL_INT) {
        fprintf(stderr, "Runtime type error: SUBSTR expects (string, int, int)\n");
        exit(1);
    }
    Value out = bi_substr(&str, (int)startv.i, (int)lenv.i);
    free_value(str);
    free_value(startv);
    free_value(lenv);
    push_value(vm, out);
    break;
}
