/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file store_local.c
 * @brief Implements the OP_STORE_LOCAL opcode for storing local variables in the VM.
 *
 * This file handles the OP_STORE_LOCAL instruction, which stores a value into a local variable
 * using its index.
 *
 * Behavior:
 * - Pops the value from the stack.
 * - Stores the value into the local variable at the specified index.
 *
 * Error Handling:
 * - Exits with an error if the index is out of bounds.
 *
 * Example:
 * // Bytecode: OP_STORE_LOCAL 0
 * // Stack before: [42]
 * // Stack after: []
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_STORE_LOCAL: {
    int slot = inst.operand;
    if (slot < 0 || slot >= MAX_FRAME_LOCALS) {
        fprintf(stderr, "Runtime error: local slot out of range\n");
        exit(1);
    }
    Value v = pop_value(vm);
    /* free previous local then move v into it */
    free_value(f->locals[slot]);
    f->locals[slot] = v;
    break;
}
