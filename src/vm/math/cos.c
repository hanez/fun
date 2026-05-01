/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file cos.c
 * @brief Implements the OP_COS opcode using C99 math.h cos().
 *
 * VM opcode snippet included by vm.c. Provides cosine function.
 *
 * Behavior:
 * - Pops one numeric operand (int or float) from the stack.
 * - Computes cos(x) in double precision.
 * - Always pushes a VAL_FLOAT result.
 *
 * Stack effect:
 * - Pop: x
 * - Push: cos(x)
 *
 * Types:
 * - Accepts VAL_INT and VAL_FLOAT.
 * - Other types cause a runtime error.
 *
 * Example:
 * - Input [0] → Output [1.0]
 */

#include <math.h>

case OP_COS: {
  Value v = pop_value(vm);
  if (v.type == VAL_INT || v.type == VAL_FLOAT) {
    double x = (v.type == VAL_FLOAT) ? v.d : (double)v.i;
    double r = cos(x);
    push_value(vm, make_float(r));
    free_value(v);
  } else {
    fprintf(stderr, "Runtime type error: COS expects number, got %s\n", value_type_name(v.type));
    exit(1);
  }
  break;
}
