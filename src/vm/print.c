/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file print.c
 * @brief Implements the OP_PRINT opcode for printing values in the VM.
 *
 * This file handles the OP_PRINT instruction, which prints the top value on the stack
 * to the output buffer. The value is popped from the stack.
 *
 * Behavior:
 * - Pops the value from the stack.
 * - Prints the value to the output buffer.
 *
 * Example:
 * // Bytecode: OP_PRINT
 * // Stack before: [42]
 * // Stack after: []
 *
 * @author Johannes Findeisen
 * @date 2025-09-16
 */

case OP_PRINT: {
    Value v = pop_value(vm);
    Value snap = deep_copy_value(&v);
    free_value(v);
    if (vm->output_count < OUTPUT_SIZE) {
        vm->output[vm->output_count++] = snap;
    } else {
        free_value(snap);
        fprintf(stderr, "Runtime error: output buffer overflow\n");
        exit(1);
    }
    break;
}
