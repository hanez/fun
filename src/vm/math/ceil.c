/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file ceil.c
 * @brief Implements the OP_CEIL opcode using C99 math.h ceil().
 *
 * VM opcode snippet included by vm.c. Provides numeric ceiling operation.
 *
 * Behavior:
 * - Pops one numeric operand (int or float) from the stack.
 * - Applies ceil(x) in double precision.
 * - If the result is an exact 64-bit integer, pushes VAL_INT; otherwise VAL_FLOAT.
 *
 * Stack effect:
 * - Pop: x
 * - Push: ceil(x)
 *
 * Types:
 * - Accepts VAL_INT and VAL_FLOAT.
 * - Other types cause a runtime error.
 *
 * Errors:
 * - Exits with an error message if the operand is not a number.
 *
 * Example:
 * - Input stack: [2.1] → Output stack: [3]
 * - Input stack: [-2.1] → Output stack: [-2]
 */

#include <math.h>

case OP_CEIL: {
  Value v = pop_value(vm);
  if (v.type == VAL_INT) {
    /* ceil(n) == n for integers */
    push_value(vm, make_int(v.i));
    free_value(v);
  } else if (v.type == VAL_FLOAT) {
    double r = ceil(v.d);
    if (r >= (double)INT64_MIN && r <= (double)INT64_MAX) {
      int64_t ii = (int64_t)r;
      if ((double)ii == r) {
        push_value(vm, make_int(ii));
      } else {
        push_value(vm, make_float(r));
      }
    } else {
      push_value(vm, make_float(r));
    }
    free_value(v);
  } else {
    fprintf(stderr, "Runtime type error: CEIL expects number, got %s\n", value_type_name(v.type));
    exit(1);
  }
  break;
}
