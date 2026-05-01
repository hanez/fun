/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

#include "value.h"

/**
 * @file array_utils.c
 * @brief Utility functions for operating on Value arrays.
 */

/**
 * @brief Check if an array Value contains a given element.
 *
 * Performs linear search using value_equals() on copied elements.
 *
 * @param v       Array Value to inspect (may be NULL).
 * @param needle  Value to search for.
 * @return 1 if found, 0 otherwise or if v is not an array.
 */
int array_contains(const Value *v, const Value *needle) {
  if (!v || v->type != VAL_ARRAY || !v->arr) return 0;
  int n = array_length(v);
  for (int i = 0; i < n; ++i) {
    Value item;
    if (array_get_copy(v, i, &item)) {
      int eq = value_equals(&item, needle);
      free_value(item);
      if (eq) return 1;
    }
  }
  return 0;
}

/**
 * @brief Find the index of the first occurrence of an element in an array.
 *
 * @param v       Array Value to search (may be NULL).
 * @param needle  Value to look for.
 * @return Zero-based index when found; -1 if not found or if v is not an array.
 */
int array_index_of(const Value *v, const Value *needle) {
  if (!v || v->type != VAL_ARRAY || !v->arr) return -1;
  int n = array_length(v);
  for (int i = 0; i < n; ++i) {
    Value item;
    if (array_get_copy(v, i, &item)) {
      int eq = value_equals(&item, needle);
      free_value(item);
      if (eq) return i;
    }
  }
  return -1;
}

/**
 * @brief Remove all elements from an array Value, freeing their contents.
 *
 * Uses array_pop() repeatedly to clear the array.
 *
 * @param v Array Value to clear (in place). No-op if not an array.
 */
void array_clear(Value *v) {
  if (!v || v->type != VAL_ARRAY || !v->arr) return;
  int n = array_length(v);
  for (int i = 0; i < n; ++i) {
    Value item;
    if (array_get_copy(v, i, &item)) {
      free_value(item);
    }
  }
  /* internal clear uses public API to reset to zero length */
  /* Since we don't expose capacity, emulate by popping elements */
  Value out;
  while (array_pop(v, &out)) {
    free_value(out);
  }
}
