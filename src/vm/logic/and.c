/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the ISC license.
 * https://opensource.org/license/isc-license-txt
 */

/**
* @file and.c
 * @brief Implements the OP_AND opcode for logical AND in the VM.
 *
 * This file handles the OP_AND instruction, which performs a logical AND operation
 * on two boolean values. The values are popped from the stack, and the result is pushed back.
 *
 * Behavior:
 * - Pops two boolean values from the stack.
 * - Performs a logical AND operation.
 * - Pushes the result (1 or 0) onto the stack.
 *
 * Error Handling:
 * - Exits with an error if the operands are not boolean values.
 *
 * Example:
 * // Bytecode: OP_AND
 * // Stack before: [1, 0]
 * // Stack after: [0]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_AND: {
    Value b = pop_value(vm);
    Value a = pop_value(vm);
    int res = value_is_truthy(&a) && value_is_truthy(&b);
    free_value(a);
    free_value(b);
    push_value(vm, make_int(res));
    break;
}
