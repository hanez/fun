/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file dup.c
 * @brief Implements the OP_DUP opcode for duplicating the top stack value in the VM.
 *
 * This file handles the OP_DUP instruction, which duplicates the top value on the stack.
 * The duplicated value is pushed back onto the stack.
 *
 * Behavior:
 * - Duplicates the top value on the stack.
 * - Pushes the duplicated value onto the stack.
 *
 * Error Handling:
 * - Exits with an error if the stack is empty.
 *
 * Example:
 * // Bytecode: OP_DUP
 * // Stack before: [42]
 * // Stack after: [42, 42]
*
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_DUP: {
    if (vm->sp < 0) {
        fprintf(stderr, "Runtime error: stack underflow for DUP\n");
        exit(1);
    }
    Value top = vm->stack[vm->sp];
    push_value(vm, copy_value(&top));
    break;
}
