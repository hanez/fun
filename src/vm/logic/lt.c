/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file lt.c
 * @brief Implements the OP_LT opcode for less-than comparison in the VM.
 *
 * This file handles the OP_LT instruction, which checks if the first value is less than the second.
 * The values are popped from the stack, and the result (1 or 0) is pushed back.
 *
 * Behavior:
 * - Pops two values from the stack.
 * - Checks if the first value is less than the second.
 * - Pushes 1 (true) or 0 (false) onto the stack.
 *
 * Error Handling:
 * - Exits with an error if the operands are of incompatible types.
 *
 * Example:
 * // Bytecode: OP_LT
 * // Stack before: [10, 42]
 * // Stack after: [1]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_LT: {
    Value b = pop_value(vm);
    Value a = pop_value(vm);
    if (a.type != VAL_INT || b.type != VAL_INT) {
        fprintf(stderr, "Runtime type error: LT expects ints, got %s and %s\n",
                value_type_name(a.type), value_type_name(b.type));
        exit(1);
    }
    Value res = make_int(a.i < b.i ? 1 : 0);
    free_value(a);
    free_value(b);
    push_value(vm, res);
    break;
}
