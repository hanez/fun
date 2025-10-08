/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-04
 */

/**
* @file exit.c
 * @brief Implements the OP_EXIT opcode to terminate the script with an exit code.
 *
 * Behavior:
 * - Pops a value from the stack (if available) and converts it to an integer exit code.
 * - Sets vm->exit_code.
 * - Stops the VM execution immediately (returns from vm_run).
 */

case OP_EXIT: {
    int code = 0;
    if (vm->sp >= 0) {
        Value v = pop_value(vm);
        if (v.type == VAL_INT) {
            code = (int)v.i;
        } else if (v.type == VAL_STRING) {
            /* best-effort parse number from string */
            code = (int)strtoll(v.s, NULL, 10);
        } else if (v.type == VAL_NIL) {
            code = 0;
        } else {
            /* unsupported type for exit; default to 0 */
            code = 0;
        }
        free_value(v);
    }
    vm->exit_code = code;
    return;
}
