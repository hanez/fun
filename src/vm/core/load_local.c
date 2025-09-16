/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the ISC license.
 * https://opensource.org/license/isc-license-txt
 */

/**
* @file load_local.c
 * @brief Implements the OP_LOAD_LOCAL opcode for loading local variables in the VM.
 *
 * This file handles the OP_LOAD_LOCAL instruction, which loads a local variable
 * onto the stack using its index.
 *
 * Behavior:
 * - Loads the local variable at the specified index.
 * - Pushes the value onto the stack.
 *
 * Error Handling:
 * - Exits with an error if the index is out of bounds.
 *
 * Example:
 * // Bytecode: OP_LOAD_LOCAL 0
 * // Stack before: []
 * // Stack after: [local_value]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_LOAD_LOCAL: {
    int slot = inst.operand;
    if (slot < 0 || slot >= MAX_FRAME_LOCALS) {
        fprintf(stderr, "Runtime error: local slot out of range\n");
        exit(1);
    }
    Value val = copy_value(&f->locals[slot]);
    push_value(vm, val);
    break;
}
