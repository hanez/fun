/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the ISC license.
 * https://opensource.org/license/isc-license-txt
 */

/**
* @file has_key.c
 * @brief Implements the OP_HAS_KEY opcode for map key checking in the VM.
 *
 * This file handles the OP_HAS_KEY instruction, which checks if a map contains
 * a specific key.
 *
 * Behavior:
 * - Pops key and map from stack
 * - Pushes 1 if key exists, 0 otherwise
 *
 * Error Handling:
 * - Exits if arguments wrong types
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_HAS_KEY: {
    Value key = pop_value(vm);
    Value m = pop_value(vm);
    if (m.type != VAL_MAP || key.type != VAL_STRING) { fprintf(stderr, "HAS_KEY expects (map, string)\n"); exit(1); }
    int ok = map_has(&m, key.s ? key.s : "");
    free_value(m); free_value(key);
    push_value(vm, make_int(ok ? 1 : 0));
    break;
}
