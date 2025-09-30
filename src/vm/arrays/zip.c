/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file zip.c
 * @brief Implements the OP_ZIP opcode for array zipping in the VM.
 *
 * This file handles the OP_ZIP instruction, which combines two arrays into
 * an array of pairs (subarrays with two elements).
 *
 * Behavior:
 * - Pops two arrays from the stack
 * - Creates new array of [a[i],b[i]] pairs
 * - Length is minimum of input array lengths
 * - Pushes resulting array onto stack
 *
 * Error Handling:
 * - Exits with error if arguments aren't arrays
 *
 * Example:
 * // Bytecode: OP_ZIP 
 * // Stack before: [[1,2], ['a','b']]
 * // Stack after: [[[1,'a'], [2,'b']]]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_ZIP: {
    Value b = pop_value(vm);
    Value a = pop_value(vm);
    if (a.type != VAL_ARRAY || b.type != VAL_ARRAY) {
        fprintf(stderr, "Runtime type error: ZIP expects (array, array)\n");
        exit(1);
    }
    Value out = bi_zip(&a, &b);
    free_value(a);
    free_value(b);
    push_value(vm, out);
    break;
}
