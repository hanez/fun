/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file or.c
 * @brief Implements the OP_OR opcode for logical OR in the VM.
 *
 * This file handles the OP_OR instruction, which performs a logical OR operation
 * on two boolean values. The values are popped from the stack, and the result is pushed back.
 *
 * Behavior:
 * - Pops two boolean values from the stack.
 * - Performs a logical OR operation.
 * - Pushes the result (1 or 0) onto the stack.
 *
 * Error Handling:
 * - Exits with an error if the operands are not boolean values.
 *
 * Example:
 * // Bytecode: OP_OR
 * // Stack before: [1, 0]
 * // Stack after: [1]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_OR: {
    Value b = pop_value(vm);
    Value a = pop_value(vm);
    int res = value_is_truthy(&a) || value_is_truthy(&b);
    free_value(a);
    free_value(b);
    push_value(vm, make_bool(res));
    break;
}
