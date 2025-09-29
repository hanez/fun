/**
* This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the ISC license.
 * https://opensource.org/license/isc-license-txt
 *
 * Added: 2025-09-29
 */

/**
 * OP_BNOT: bitwise NOT (uint32 32-bit)
 * pops: a
 * pushes: (uint32_t)(~a)
 */
case OP_BNOT: {
    Value va = pop_value(vm);
    uint32_t a = (va.type == VAL_INT) ? (uint32_t)va.i : 0u;
    uint32_t r = ~a;
    free_value(va);
    push_value(vm, make_int((int64_t)(uint64_t)r));
    break;
}
