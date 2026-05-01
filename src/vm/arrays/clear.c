/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file clear.c
 * @brief Implements the OP_CLEAR opcode for clearing arrays in the VM.

 * Handles the OP_CLEAR instruction, which removes all elements from an array.
 * The array is popped from the stack; the opcode pushes an integer result
 * (currently 0) as an acknowledgement.

 * Behavior:
 * - Pops the array from the stack.
 * - Clears all elements from the array (array becomes empty in place).
 * - Pushes 0 (integer) to acknowledge success.

 * Error Handling:
 * - Exits with a runtime error if the operand is not an array.

 * Example:
 * // Bytecode: OP_CLEAR
 * // Stack before: [[10, 20, 30]]
 * // Stack after: [0]

 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_CLEAR: {
  Value arr = pop_value(vm);
  if (arr.type != VAL_ARRAY) {
    fprintf(stderr, "Runtime type error: CLEAR expects array\n");
    exit(1);
  }
  array_clear(&arr);
  free_value(arr);
  push_value(vm, make_int(0));
  break;
}
