/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file return.c
 * @brief Implements the OP_RETURN opcode for returning from a function in the VM.
 *
 * This file handles the OP_RETURN instruction, which returns from the current function
 * and optionally pushes a return value onto the stack.
 *
 * Behavior:
 * - Pops the optional return value from the stack.
 * - Returns to the caller frame.
 * - Pushes the return value onto the stack (if any).
 *
 * Error Handling:
 * - Exits with an error if the frame stack is empty.
 *
 * Example:
 * // Bytecode: OP_RETURN
 * // Stack before: [42]
 * // Stack after: [42]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_RETURN: {
    Value retv;
    if (vm->sp >= 0) retv = pop_value(vm);
    else retv = make_nil();
    vm_pop_frame(vm);
    push_value(vm, retv);
    break;
}
