/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file bxor.c
 * @brief Implements the OP_BXOR opcode bitwise XOR (uint32).
 *
 * This file is an opcode snippet included by vm.c. It implements a
 * 32-bit unsigned bitwise XOR of two integer operands from the VM stack.
 * 
 * Stack effects:
 *  - pops: b, a
 *  - pushes: (uint32_t)(a ^ b)
 *
 * Notes:
 *  - Operands are taken as 32-bit unsigned integers when of type VAL_INT;
 *    non-integer values are treated as 0.
 *  - Result is pushed back as VAL_INT, preserving 32-bit value in the
 *    low bits of the 64-bit integer storage.
 */

case OP_BXOR: {
  Value vb = pop_value(vm);
  Value va = pop_value(vm);
  uint32_t a = (va.type == VAL_INT) ? (uint32_t)va.i : 0u;
  uint32_t b = (vb.type == VAL_INT) ? (uint32_t)vb.i : 0u;
  uint32_t r = a ^ b;
  free_value(vb);
  free_value(va);
  push_value(vm, make_int((int64_t)(uint64_t)r));
  break;
}
