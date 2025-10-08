/**
* This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-09-29
 */

/**
 * OP_ROTR: rotate right (uint32)
 * pops: s, a
 * pushes: rotr32(a, s)
 */
case OP_ROTR: {
    Value vs = pop_value(vm);
    Value va = pop_value(vm);
    uint32_t a = (va.type == VAL_INT) ? (uint32_t)va.i : 0u;
    uint32_t s = (vs.type == VAL_INT) ? (uint32_t)vs.i : 0u;
    s &= 31u;
    uint32_t r = (s == 0u) ? a : ((a >> s) | (a << (32u - s)));
    free_value(vs);
    free_value(va);
    push_value(vm, make_int((int64_t)(uint64_t)r));
    break;
}
