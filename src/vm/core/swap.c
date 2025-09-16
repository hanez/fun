/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the ISC license.
 * https://opensource.org/license/isc-license-txt
 */

/**
* @file swap.c
 * @brief Implements the OP_SWAP opcode for stack manipulation in the VM.
 *
 * This file handles the OP_SWAP instruction, which swaps the top two values
 * on the stack.
 *
 * Behavior:
 * - Swaps stack[sp] and stack[sp-1]
 * - No type checking
 *
 * Error Handling:
 * - Exits if stack underflow
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_SWAP: {
    if (vm->sp < 1) {
        fprintf(stderr, "Runtime error: stack underflow for SWAP\n");
        exit(1);
    }
    Value a = vm->stack[vm->sp];
    Value b = vm->stack[vm->sp - 1];
    vm->stack[vm->sp] = b;
    vm->stack[vm->sp - 1] = a;
    break;
}
