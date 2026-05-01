/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file shl.c
 * @brief Implements the OP_SHL opcode logical left shift (uint32).
 *
 * Opcode snippet included by vm.c. Performs a 32-bit unsigned logical left
 * shift of an integer operand by a masked shift count.
 * 
 * Stack effects:
 *  - pops: s, a
 *  - pushes: (uint32_t)(a << (s & 31))
 *
 * Notes:
 *  - Both a (value) and s (shift) are taken from VAL_INT; non-integers are 0.
 *  - The shift count is masked to 0..31. A zero shift returns a unchanged.
 *  - Result is pushed as VAL_INT with the 32-bit value preserved in the low bits.
 */

case OP_SHL: {
  Value vs = pop_value(vm);
  Value va = pop_value(vm);
  uint32_t a = (va.type == VAL_INT) ? (uint32_t)va.i : 0u;
  uint32_t s = (vs.type == VAL_INT) ? (uint32_t)vs.i : 0u;
  s &= 31u;
  uint32_t r = (s == 0u) ? a : (a << s);
  free_value(vs);
  free_value(va);
  push_value(vm, make_int((int64_t)(uint64_t)r));
  break;
}
