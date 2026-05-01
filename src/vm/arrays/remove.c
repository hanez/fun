/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file remove.c
 * @brief Implements the OP_REMOVE opcode for removing elements from arrays in the VM.
 *
 * Handles the OP_REMOVE instruction, which removes an element from an array at the
 * specified index. The array and index are popped from the stack; the removed
 * element is pushed back onto the stack.
 *
 * Behavior:
 * - Pops the index and array from the stack.
 * - Removes the element at the specified index from the array.
 * - Pushes the removed element onto the stack.
 *
 * Error Handling:
 * - Exits with a runtime error if the container is not an array or index is not an int.
 * - Exits with a runtime error if the index is out of bounds.
 *
 * Example:
 * // Bytecode: OP_REMOVE
 * // Stack before: [1, [10, 20, 30]]
 * // Stack after: [20]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_REMOVE: {
  Value idx = pop_value(vm);
  Value arr = pop_value(vm);
  if (arr.type != VAL_ARRAY || idx.type != VAL_INT) {
    fprintf(stderr, "Runtime type error: ARR_REMOVE expects (array, int)\n");
    exit(1);
  }
  Value out;
  if (!array_remove(&arr, (int)idx.i, &out)) {
    fprintf(stderr, "Runtime error: remove index out of range\n");
    exit(1);
  }
  free_value(arr);
  free_value(idx);
  push_value(vm, out);
  break;
}
