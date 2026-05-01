/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file sub.c
 * @brief Implements the OP_SUB opcode (numeric subtraction) in the VM.
 *
 * Handles the OP_SUB instruction, subtracting two numeric operands and
 * pushing the result. If either operand is a float, subtraction is
 * performed in double precision; otherwise it is 64-bit integer
 * subtraction.
 *
 * Behavior:
 * - Pops two values from the stack.
 * - If any operand is VAL_FLOAT, computes (double)a - (double)b and pushes a VAL_FLOAT.
 * - Else computes a.i - b.i and pushes a VAL_INT.
 *
 * Error Handling:
 * - Raises a runtime error and aborts execution if operands are not numeric.
 *
 * Example:
 * // Bytecode: OP_SUB
 * // Stack before: [10, 4]
 * // Stack after: [6]
 * // Stack before: [10.0, 3]
 * // Stack after: [7.0]
 */

case OP_SUB: {
  Value b = pop_value(vm);
  Value a = pop_value(vm);
  if ((a.type == VAL_INT || a.type == VAL_FLOAT) && (b.type == VAL_INT || b.type == VAL_FLOAT)) {
    if (a.type == VAL_FLOAT || b.type == VAL_FLOAT) {
      double da = (a.type == VAL_FLOAT) ? a.d : (double)a.i;
      double db = (b.type == VAL_FLOAT) ? b.d : (double)b.i;
      Value res = make_float(da - db);
      free_value(a);
      free_value(b);
      push_value(vm, res);
    } else {
      Value res = make_int(a.i - b.i);
      free_value(a);
      free_value(b);
      push_value(vm, res);
    }
  } else {
    fprintf(stderr, "Runtime type error: SUB expects numbers, got %s and %s\n",
            value_type_name(a.type), value_type_name(b.type));
    exit(1);
  }
  break;
}
