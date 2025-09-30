/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file mul.c
 * @brief Implements the OP_MUL opcode for integer multiplication in the VM.
 *
 * This file handles the OP_MUL instruction, which performs integer multiplication
 * on two integer values popped from the stack and pushes the result back onto the stack.
 *
 * Behavior:
 * - Pops two integer values from the stack.
 * - Performs integer multiplication (`a * b`).
 * - Pushes the result back onto the stack.
 *
 * Error Handling:
 * - Exits with an error if the operands are not integers.
 *
 * Example:
 * // Bytecode: OP_MUL
 * // Stack before: [3, 4]
 * // Stack after: [12]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_MUL: {
    Value b = pop_value(vm);
    Value a = pop_value(vm);
    if (a.type != VAL_INT || b.type != VAL_INT) {
        fprintf(stderr, "Runtime type error: MUL expects ints, got %s and %s\n",
                value_type_name(a.type), value_type_name(b.type));
        exit(1);
    }
    Value res = make_int(a.i * b.i);
    free_value(a);
    free_value(b);
    push_value(vm, res);
    break;
}
