/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file clamp.c
 * @brief Implements the OP_CLAMP opcode for value clamping in the VM.
 *
 * This file handles the OP_CLAMP instruction, which clamps a value between
 * a minimum and maximum.
 *
 * Behavior:
 * - Pops x, lo, hi from stack
 * - Pushes x clamped to [lo,hi]
 *
 * Error Handling:
 * - Exits if arguments wrong types
 * - Handles hi < lo case
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_CLAMP: {
    Value hi = pop_value(vm);
    Value lo = pop_value(vm);
    Value x = pop_value(vm);
    if (x.type != VAL_INT || lo.type != VAL_INT || hi.type != VAL_INT) {
        fprintf(stderr, "CLAMP expects ints\n");
        exit(1);
    }
    int64_t v = x.i;
    if (v < lo.i) v = lo.i;
    if (v > hi.i) v = hi.i;
    push_value(vm, make_int(v));
    free_value(x);
    free_value(lo);
    free_value(hi);
    break;
}
