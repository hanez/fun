/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file uclamp.c
 * @brief Implements the OP_UCLAMP opcode for unsigned N-bit wrapping.
 *
 * This VM opcode masks an integer to the lower N bits (N taken from the
 * instruction operand), effectively performing an unsigned wrap-around into
 * the range [0 .. 2^N - 1].
 *
 * Stack contract:
 * - Pops: value (int)
 * - Pushes: value (int)
 */

case OP_UCLAMP: {
  /* Unsigned wrap to N bits: mask lower N bits (operand = bits) */
  Value v = pop_value(vm);
  int bits = inst.operand;
  int64_t vi = (v.type == VAL_INT) ? v.i : 0;

  uint64_t mask;
  if (bits <= 0) {
    mask = 0ULL;
  } else if (bits >= 64) {
    mask = UINT64_MAX;
  } else {
    mask = (1ULL << bits) - 1ULL;
  }

  uint64_t wrapped = ((uint64_t)vi) & mask;
  push_value(vm, make_int((int64_t)wrapped));
  free_value(v);
  break;
}
