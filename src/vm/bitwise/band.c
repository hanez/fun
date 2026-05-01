/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

 /**
  * @file band.c
  * @brief Implements the OP_BAND opcode bitwise AND (uint32).
  *
  * Opcode snippet included by vm.c. Performs a 32-bit unsigned bitwise AND
  * on two integer operands from the VM stack.
  * 
  * Stack effects:
  *  - pops: b, a
  *  - pushes: (uint32_t)(a & b)
  *
  * Notes:
  *  - Operands are interpreted as 32-bit unsigned when of type VAL_INT;
  *    non-integer values are treated as 0.
  *  - The result is pushed as VAL_INT with the 32-bit value preserved in the
  *    low bits.
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
