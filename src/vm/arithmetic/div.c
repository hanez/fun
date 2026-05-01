/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file div.c
 * @brief Implements the OP_DIV opcode (numeric division) in the VM.
 *
 * Handles the OP_DIV instruction, dividing two numeric operands and pushing
 * the result. If either operand is a float, division is performed in double
 * precision and a VAL_FLOAT is produced; otherwise integer division is
 * used and a VAL_INT is produced.
 *
 * Behavior:
 * - Pops two values from the stack.
 * - If any operand is VAL_FLOAT, computes (double)a / (double)b and pushes a VAL_FLOAT.
 * - Else computes a.i / b.i and pushes a VAL_INT.
 *
 * Error Handling:
 * - Raises a runtime error and aborts execution if operands are not numeric.
 * - Raises a runtime error on division by zero (both integer and floating cases).
 *
 * Example:
 * // Bytecode: OP_DIV
 * // Stack before: [10, 2]
 * // Stack after: [5]
 * // Stack before: [5.0, 2]
 * // Stack after: [2.5]
 */

case OP_DIV: {
  Value b = pop_value(vm);
  Value a = pop_value(vm);
  if ((a.type == VAL_INT || a.type == VAL_FLOAT) && (b.type == VAL_INT || b.type == VAL_FLOAT)) {
    if (a.type == VAL_FLOAT || b.type == VAL_FLOAT) {
      double da = (a.type == VAL_FLOAT) ? a.d : (double)a.i;
      double db = (b.type == VAL_FLOAT) ? b.d : (double)b.i;
      if (db == 0.0) {
        vm_raise_error(vm, "division by zero");
        break;
      }
      Value res = make_float(da / db);
      free_value(a);
      free_value(b);
      push_value(vm, res);
    } else {
      if (b.i == 0) {
        vm_raise_error(vm, "division by zero");
        break;
      }
      Value res = make_int(a.i / b.i);
      free_value(a);
      free_value(b);
      push_value(vm, res);
    }
  } else {
    fprintf(stderr, "Runtime type error: DIV expects numbers, got %s and %s\n",
            value_type_name(a.type), value_type_name(b.type));
    exit(1);
  }
  break;
}
