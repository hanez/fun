/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file pop.c
 * @brief Implements the OP_POP opcode for removing the top stack value in the VM.
 *
 * This file handles the OP_POP instruction, which removes the top value from the stack.
 *
 * Behavior:
 * - Removes the top value from the stack.
 *
 * Error Handling:
 * - Exits with an error if the stack is empty.
 *
 * Example:
 * // Bytecode: OP_POP
 * // Stack before: [42]
 * // Stack after: []
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_POP: {
    if (vm->sp < 0) {
        fprintf(stderr, "Runtime error: stack underflow for POP\n");
        exit(1);
    }
    Value v = pop_value(vm);
    free_value(v);
    break;
}
