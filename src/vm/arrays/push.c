/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file push.c
 * @brief Implements the OP_PUSH opcode for appending elements to arrays in the VM.
 *
 * Handles the OP_PUSH instruction, which appends a value to the end of an array.
 * The array and value are popped from the stack; the opcode pushes the new array
 * length (integer) as a result.
 *
 * Behavior:
 * - Pops the value and array from the stack.
 * - Appends the value to the end of the array.
 * - Pushes the new length of the array onto the stack.
 *
 * Error Handling:
 * - Exits with a runtime error if the first operand is not an array.
 * - Exits with a runtime error if memory allocation fails.
 *
 * Example:
 * // Bytecode: OP_PUSH
 * // Stack before: [42, [10, 20, 30]]
 * // Stack after: [4]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_PUSH: {
  Value v = pop_value(vm);
  Value arr = pop_value(vm);
  if (arr.type != VAL_ARRAY) {
    fprintf(stderr, "Runtime type error: ARR_PUSH expects array\n");
    exit(1);
  }
  int n = array_push(&arr, v);
  if (n < 0) {
    fprintf(stderr, "Runtime error: push failed (OOM?)\n");
    exit(1);
  }
  free_value(arr);
  push_value(vm, make_int(n));
  break;
}
