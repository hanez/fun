/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file not.c
 * @brief Implements the OP_NOT opcode for logical NOT in the VM.
 *
 * This file handles the OP_NOT instruction, which performs a logical NOT operation
 * on a boolean value. The value is popped from the stack, and the result is pushed back.
 *
 * Behavior:
 * - Pops a boolean value from the stack.
 * - Performs a logical NOT operation.
 * - Pushes the result (1 or 0) onto the stack.
 *
 * Error Handling:
 * - Exits with an error if the operand is not a boolean value.
 *
 * Example:
 * // Bytecode: OP_NOT
 * // Stack before: [0]
 * // Stack after: [1]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_NOT: {
    Value v = pop_value(vm);
    int res = !value_is_truthy(&v);
    free_value(v);
    push_value(vm, make_int(res));
    break;
}
