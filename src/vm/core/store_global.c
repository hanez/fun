/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file store_global.c
 * @brief Implements the OP_STORE_GLOBAL opcode for storing global variables in the VM.
 *
 * This file handles the OP_STORE_GLOBAL instruction, which stores a value into a global variable
 * using its index.
 *
 * Behavior:
 * - Pops the value from the stack.
 * - Stores the value into the global variable at the specified index.
 *
 * Error Handling:
 * - Exits with an error if the index is out of bounds.
 *
 * Example:
 * // Bytecode: OP_STORE_GLOBAL 0
 * // Stack before: [42]
 * // Stack after: []
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_STORE_GLOBAL: {
    int idx = inst.operand;
    if (idx < 0 || idx >= MAX_GLOBALS) {
        fprintf(stderr, "Runtime error: global index out of range\n");
        exit(1);
    }
    Value v = pop_value(vm);
#ifdef FUN_DEBUG
    fprintf(stderr, "DEBUG STORE_GLOBAL[%d]: new.type=%d\n", idx, v.type);
#endif
    free_value(vm->globals[idx]);
    vm->globals[idx] = v;
    break;
}
