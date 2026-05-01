/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file ror.c
 * @brief Implements the OP_ROTR opcode (rotate-right).
 *
 * Opcode snippet included by vm.c. Performs a 32-bit unsigned rotate-right of
 * an integer operand by a masked rotation count.
 */

/**
 * OP_ROTR: rotate right (uint32)
 *
 * Stack effects:
 *  - pops: s, a
 *  - pushes: (a >> s) | (a << (32 - s)), with s masked to 0..31
 *
 * Notes:
 *  - Both a (value) and s (count) are taken from VAL_INT; non-integers are 0.
 *  - The rotation count is masked to 0..31. A zero rotation returns a unchanged.
 *  - Result is pushed as VAL_INT with the 32-bit value preserved in the low bits.
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
