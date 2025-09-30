/**
* This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-09-29
 */

/**
 * OP_BAND: bitwise AND (uint32 32-bit)
 * pops: b, a
 * pushes: (uint32_t)(a & b)
 */
case OP_BAND: {
    Value vb = pop_value(vm);
    Value va = pop_value(vm);
    uint32_t a = (va.type == VAL_INT) ? (uint32_t)va.i : 0u;
    uint32_t b = (vb.type == VAL_INT) ? (uint32_t)vb.i : 0u;
    uint32_t r = a & b;
    free_value(vb);
    free_value(va);
    push_value(vm, make_int((int64_t)(uint64_t)r));
    break;
}
