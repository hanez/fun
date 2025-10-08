/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file mod.c
 * @brief Implements the OP_MOD opcode for modulo operation in the VM.
 *
 * This file handles the OP_MOD instruction, which computes the modulo of two integer values.
 * The values are popped from the stack, and the result is pushed back.
 *
 * Behavior:
 * - Pops two integer values from the stack.
 * - Computes the modulo of the first value by the second.
 * - Pushes the result onto the stack.
 *
 * Error Handling:
 * - Exits with an error if the operands are not integers.
 * - Exits with an error if the second operand is zero.
 *
 * Example:
 * // Bytecode: OP_MOD
 * // Stack before: [10, 3]
 * // Stack after: [1]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_MOD: {
    Value b = pop_value(vm);
    Value a = pop_value(vm);
    if (a.type != VAL_INT || b.type != VAL_INT) {
        fprintf(stderr, "Runtime type error: MOD expects ints, got %s and %s\n",
                value_type_name(a.type), value_type_name(b.type));
        exit(1);
    }
    if (b.i == 0) {
        fprintf(stderr, "Runtime error: modulo by zero\n");
        exit(1);
    }
    Value res = make_int(a.i % b.i);
    free_value(a);
    free_value(b);
    push_value(vm, res);
    break;
}
