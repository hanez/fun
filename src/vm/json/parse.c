/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file parse.c
 * @brief VM opcode snippet for parsing a JSON string into a Fun Value.
 *
 * Implements the OP_JSON_PARSE instruction. Expects a string (or any value
 * convertible to string) on the stack, parses it with json-c, converts the
 * resulting json_object to a Fun Value and pushes it.
 *
 * Build gating: compiled only when FUN_WITH_JSON is enabled. Otherwise the
 * opcode consumes its argument and pushes Nil.
 *
 * Stack effect (with FUN_WITH_JSON):
 * - Pops: text (any; converted to string)
 * - Pushes: Value converted from JSON, or Nil on error
 *
 * Errors and edge cases:
 * - If allocation fails, tokenization fails, or the text is not valid JSON,
 *   the opcode pushes Nil.
 * - The temporary json-c objects are released after conversion; ownership of
 *   the pushed Fun Value follows normal VM semantics.
 */

/* JSON_PARSE */
case OP_JSON_PARSE: {
#ifdef FUN_WITH_JSON
  Value text = pop_value(vm);
  char *s = value_to_string_alloc(&text);
  free_value(text);
  if (!s) {
    push_value(vm, make_nil());
    break;
  }
  struct json_tokener *tok = json_tokener_new();
  json_object *root = json_tokener_parse_ex(tok, s, (int)strlen(s));
  enum json_tokener_error jerr = json_tokener_get_error(tok);
  json_tokener_free(tok);
  free(s);
  if (jerr != json_tokener_success) {
    push_value(vm, make_nil());
  } else {
    Value v = json_to_fun(root);
    push_value(vm, v);
    json_object_put(root);
  }
#else
  /* Fallback when JSON is disabled: consume arg, push Nil */
  Value drop = pop_value(vm);
  free_value(drop);
  push_value(vm, make_nil());
#endif
  break;
}
