/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file from_file.c
 * @brief VM opcode snippet for loading a JSON document from a file.
 *
 * This snippet is included by vm.c and implements the OP_JSON_FROM_FILE
 * instruction. It expects a path on the VM stack, reads the file using
 * json-c, converts the resulting json_object tree into a Fun Value, and
 * pushes that Value back on the VM stack.
 *
 * Build gating: compiled only when FUN_WITH_JSON is enabled (json-c
 * available). When disabled, the opcode consumes its argument (if any)
 * and pushes Nil.
 *
 * Stack effect (with FUN_WITH_JSON):
 * - Pops: path (any; converted to string)
 * - Pushes: Value converted from JSON, or Nil on error
 *
 * Errors and edge cases:
 * - If the path cannot be converted to a C string or json_object_from_file
 *   fails (e.g., missing file, invalid JSON), the opcode pushes Nil.
 * - The created json_object is released after conversion; ownership of the
 *   pushed Fun Value follows normal VM semantics.
 */

/* JSON_FROM_FILE */
case OP_JSON_FROM_FILE: {
#ifdef FUN_WITH_JSON
  Value vpath = pop_value(vm);
  char *path = value_to_string_alloc(&vpath);
  free_value(vpath);
  if (!path) {
    push_value(vm, make_nil());
    break;
  }
  json_object *root = json_object_from_file(path);
  free(path);
  if (!root) {
    push_value(vm, make_nil());
    break;
  }
  Value v = json_to_fun(root);
  push_value(vm, v);
  json_object_put(root);
#else
  Value vpath = pop_value(vm);
  free_value(vpath);
  push_value(vm, make_nil());
#endif
  break;
}
