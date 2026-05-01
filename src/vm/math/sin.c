/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file sin.c
 * @brief Implements the OP_SIN opcode using C99 math.h sin().
 *
 * VM opcode snippet included by vm.c. Provides sine function.
 *
 * Behavior:
 * - Pops one numeric operand (int or float) from the stack.
 * - Computes sin(x) in double precision.
 * - Always pushes a VAL_FLOAT result.
 *
 * Stack effect:
 * - Pop: x
 * - Push: sin(x)
 *
 * Types:
 * - Accepts VAL_INT and VAL_FLOAT; others cause a runtime error.
 *
 * Example:
 * - Input [0] → Output [0.0]
 */

#include <math.h>

case OP_SIN: {
  Value v = pop_value(vm);
  if (v.type == VAL_INT || v.type == VAL_FLOAT) {
    double x = (v.type == VAL_FLOAT) ? v.d : (double)v.i;
    double r = sin(x);
    push_value(vm, make_float(r));
    free_value(v);
  } else {
    fprintf(stderr, "Runtime type error: SIN expects number, got %s\n", value_type_name(v.type));
    exit(1);
  }
  break;
}
