/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file to_file.c
 * @brief VM opcode snippet for writing a Fun Value as JSON to a file.
 *
 * Implements the OP_JSON_TO_FILE instruction. Expects a pretty-print flag,
 * a Value to serialize, and a path. Serializes the Value via json-c and
 * writes it to the specified file path.
 *
 * Build gating: compiled only when FUN_WITH_JSON is enabled. Otherwise the
 * opcode consumes three arguments and pushes 0 (failure).
 *
 * Stack effect (with FUN_WITH_JSON):
 * - Pops: pretty (bool/int), value (any), path (any; converted to string)
 * - Pushes: int (1 on success, 0 on failure)
 *
 * Errors and edge cases:
 * - If the path cannot be converted to a C string or file writing fails,
 *   the opcode pushes 0.
 * - Pretty printing selects JSON_C_TO_STRING_PRETTY; otherwise plain output.
 */

/* JSON_TO_FILE */
case OP_JSON_TO_FILE: {
#ifdef FUN_WITH_JSON
  Value vpretty = pop_value(vm);
  Value any = pop_value(vm);
  Value vpath = pop_value(vm);
  char *path = value_to_string_alloc(&vpath);
  int pretty = (vpretty.type == VAL_BOOL || vpretty.type == VAL_INT) ? (vpretty.i != 0) : 0;
  free_value(vpretty);
  free_value(vpath);
  if (!path) {
    free_value(any);
    push_value(vm, make_int(0));
    break;
  }
  json_object *j = fun_to_json(&any);
  int flags = pretty ? JSON_C_TO_STRING_PRETTY : JSON_C_TO_STRING_PLAIN;
  int rc = json_object_to_file_ext(path, j, flags);
  json_object_put(j);
  free(path);
  free_value(any);
  push_value(vm, make_int(rc == 0 ? 1 : 0));
#else
  Value vpretty = pop_value(vm);
  free_value(vpretty);
  Value any = pop_value(vm);
  free_value(any);
  Value vpath = pop_value(vm);
  free_value(vpath);
  push_value(vm, make_int(0));
#endif
  break;
}
