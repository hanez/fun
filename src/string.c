/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file string.c
 * @brief String built-ins wrappers used by VM opcodes.
 *
 * Provides small wrapper functions around core string utilities to operate on
 * the Value type used by the VM (split, join, substr, find).
 */

#include "value.h"
#include <stdlib.h>

/* String built-ins wrappers used by VM opcodes */

/**
 * @brief Split a string by a separator into an array Value.
 *
 * Safely extracts C strings from the provided `Value` arguments. If either
 * argument is NULL, not of type `VAL_STRING`, or has a NULL `s` pointer,
 * an empty string ("") is used instead.
 *
 * @param str Input string (`VAL_STRING`) to split. May be NULL.
 * @param sep Separator string (`VAL_STRING`). May be NULL. If empty,
 *            behavior is defined by `string_split_to_array`.
 * @return A `Value` representing an array of substrings (each a `VAL_STRING`).
 *         The exact array layout and split semantics are delegated to
 *         `string_split_to_array`.
 */
Value bi_split(const Value *str, const Value *sep) {
  const char *s = (str && str->type == VAL_STRING && str->s) ? str->s : "";
  const char *p = (sep && sep->type == VAL_STRING && sep->s) ? sep->s : "";
  return string_split_to_array(s, p);
}

/**
 * @brief Join an array of strings with a separator into a single string Value.
 *
 * Extracts the separator C string from `sep` if it is a `VAL_STRING`,
 * otherwise uses an empty string (""). The join operation is performed by
 * `array_join_with_sep`.
 *
 * @param arr Array `Value` expected to contain strings. Semantics for
 *            non-string elements are defined by `array_join_with_sep`.
 * @param sep Separator string (`VAL_STRING`). May be NULL; defaults to empty.
 * @return A `VAL_STRING` `Value` with the joined result. Never returns a NULL
 *         `Value`; if joining fails, an empty string is returned.
 */
Value bi_join(const Value *arr, const Value *sep) {
  const char *p = (sep && sep->type == VAL_STRING && sep->s) ? sep->s : "";
  char *s = array_join_with_sep(arr, p);
  Value out = make_string(s ? s : "");
  if (s) free(s);
  return out;
}

/**
 * @brief Extract a substring from a string `Value`.
 *
 * If `str` is not a `VAL_STRING` or is NULL, an empty source string is used.
 * The actual substring extraction semantics (e.g., handling of negative or
 * out-of-range indices) are delegated to `string_substr`.
 *
 * @param str Source string (`VAL_STRING`). May be NULL.
 * @param start Zero-based start index.
 * @param len Maximum number of characters to include in the substring.
 * @return A `VAL_STRING` `Value` containing the substring. Returns an empty
 *         string if extraction fails or inputs are treated as empty.
 */
Value bi_substr(const Value *str, int start, int len) {
  const char *s = (str && str->type == VAL_STRING && str->s) ? str->s : "";
  char *sub = string_substr(s, start, len);
  Value out = make_string(sub ? sub : "");
  if (sub) free(sub);
  return out;
}

/**
 * @brief Find the first occurrence of a needle inside a haystack string.
 *
 * If either argument is not a `VAL_STRING` or is NULL, it is treated as an
 * empty string (""). The search is performed by `string_find`.
 *
 * @param hay Haystack string (`VAL_STRING`). May be NULL.
 * @param needle Needle string (`VAL_STRING`). May be NULL.
 * @return The zero-based index of the first occurrence of `needle` in `hay`,
 *         or -1 if not found. Exact semantics are delegated to `string_find`.
 */
int bi_find(const Value *hay, const Value *needle) {
  const char *h = (hay && hay->type == VAL_STRING && hay->s) ? hay->s : "";
  const char *n = (needle && needle->type == VAL_STRING && needle->s) ? needle->s : "";
  return string_find(h, n);
}
