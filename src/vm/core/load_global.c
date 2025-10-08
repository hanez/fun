/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file load_global.c
 * @brief Implements the OP_LOAD_GLOBAL opcode for loading global variables in the VM.
 *
 * This file handles the OP_LOAD_GLOBAL instruction, which loads a global variable
 * onto the stack using its index.
 *
 * Behavior:
 * - Loads the global variable at the specified index.
 * - Pushes the value onto the stack.
 *
 * Error Handling:
 * - Exits with an error if the index is out of bounds.
 *
 * Example:
 * // Bytecode: OP_LOAD_GLOBAL 0
 * // Stack before: []
 * // Stack after: [global_value]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_LOAD_GLOBAL: {
    int idx = inst.operand;
    if (idx < 0 || idx >= MAX_GLOBALS) {
        fprintf(stderr, "Runtime error: global index out of range\n");
        exit(1);
    }
#ifdef FUN_DEBUG
    fprintf(stderr, "DEBUG LOAD_GLOBAL[%d]: type=%d\n", idx, vm->globals[idx].type);
#endif
    push_value(vm, copy_value(&vm->globals[idx]));
    break;
}
