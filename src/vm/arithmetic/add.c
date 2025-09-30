/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file add.c
 * @brief Implements the OP_ADD opcode for arithmetic and string concatenation in the VM.
 *
 * This file handles the OP_ADD instruction, which performs addition or concatenation
 * depending on the types of the operands:
 * - Integers: Adds two integers.
 * - Strings: Concatenates two strings.
 * - Arrays: Concatenates two arrays.
 *
 * Behavior:
 * - Pops two values from the stack.
 * - Performs the operation based on the types of the operands.
 * - Pushes the result back onto the stack.
 *
 * Error Handling:
 * - Exits with an error if the operands are of incompatible types.
 * - Exits with an error if memory allocation fails during string concatenation.
 *
 * Example:
 * // Bytecode: OP_ADD
 * // Stack before: [2, 3]
 * // Stack after: [5]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_ADD: {
    Value b = pop_value(vm);
    Value a = pop_value(vm);
    if (a.type == VAL_INT && b.type == VAL_INT) {
        Value res = make_int(a.i + b.i);
        free_value(a);
        free_value(b);
        push_value(vm, res);
    } else if (a.type == VAL_STRING && b.type == VAL_STRING) {
        const char *sa = a.s ? a.s : "";
        const char *sb = b.s ? b.s : "";
        size_t la = strlen(sa);
        size_t lb = strlen(sb);
        char *buf = (char*)malloc(la + lb + 1);
        if (!buf) {
            fprintf(stderr, "Runtime error: out of memory during string concatenation\n");
            exit(1);
        }
        memcpy(buf, sa, la);
        memcpy(buf + la, sb, lb);
        buf[la + lb] = '\0';
        Value res;
        res.type = VAL_STRING;
        res.s = buf;
        free_value(a);
        free_value(b);
        push_value(vm, res);
    } else if (a.type == VAL_ARRAY && b.type == VAL_ARRAY) {
        Value res = array_concat(&a, &b);
        free_value(a);
        free_value(b);
        push_value(vm, res);
    } else {
        fprintf(stderr, "Runtime type error: ADD expects both ints, both strings, or both arrays, got %s and %s\n",
                value_type_name(a.type), value_type_name(b.type));
        exit(1);
    }
    break;
}
