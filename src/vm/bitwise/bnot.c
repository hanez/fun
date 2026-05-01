/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

 /**
  * @file bnot.c
  * @brief Implements the OP_BNOT opcode (bitwise NOT).
  *
  * Opcode snippet included by vm.c. Performs a 32-bit unsigned bitwise NOT
  * on a single integer operand from the VM stack.
  */

/**
 * OP_BNOT: bitwise NOT (uint32)
 *
 * Stack effects:
 *  - pops: a
 *  - pushes: (uint32_t)(~a)
 *
 * Notes:
 *  - Operand is interpreted as 32-bit unsigned when of type VAL_INT;
 *    non-integer values are treated as 0.
 *  - The result is pushed as VAL_INT with the 32-bit value preserved in the
 *    low bits.
 */
case OP_BNOT: {
  Value va = pop_value(vm);
  uint32_t a = (va.type == VAL_INT) ? (uint32_t)va.i : 0u;
  uint32_t r = ~a;
  free_value(va);
  push_value(vm, make_int((int64_t)(uint64_t)r));
  break;
}
