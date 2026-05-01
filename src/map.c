/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file map.c
 * @brief Simple string-keyed map implementation backing VAL_MAP Values.
 */

#include "value.h"
#include <stdlib.h>
#include <string.h>

/* Internal Map definition; Value holds struct Map* */
typedef struct Map {
  int refcount;
  int count;
  int cap;
  char **keys; /* each key owned here */
  Value *vals; /* each value owned here */
} Map;

/**
 * @brief Construct a new empty map Value.
 *
 * Allocates an internal Map structure with refcount=1 and zero capacity.
 *
 * @return A Value of type VAL_MAP on success, or VAL_NIL on allocation failure.
 */
Value make_map_empty(void) {
  Map *m = (Map *)malloc(sizeof(Map));
  if (!m) return make_nil();
  m->refcount = 1;
  m->count = 0;
  m->cap = 0;
  m->keys = NULL;
  m->vals = NULL;
  Value v;
  v.type = VAL_MAP;
  v.map = (struct Map *)m;
  return v;
}

/**
 * @brief Ensure the map has capacity for at least need elements.
 * @param m    Internal map pointer (must not be NULL).
 * @param need Required capacity.
 * @return 1 on success, 0 on allocation failure.
 */
static int map_ensure_cap(Map *m, int need) {
  if (m->cap >= need) return 1;
  int ncap = m->cap == 0 ? 4 : m->cap * 2;
  while (ncap < need)
    ncap *= 2;
  char **nkeys = (char **)realloc(m->keys, sizeof(char *) * ncap);
  Value *nvals = (Value *)realloc(m->vals, sizeof(Value) * ncap);
  if (!nkeys || !nvals) return 0;
  m->keys = nkeys;
  m->vals = nvals;
  m->cap = ncap;
  return 1;
}

/**
 * @brief Insert or replace a key in the map.
 *
 * On success, ownership of v transfers into the map. On failure, v is freed.
 *
 * @param vm  Target Value of type VAL_MAP.
 * @param key NUL-terminated key string (copied into the map).
 * @param v   Value to store; consumed on success.
 * @return 1 on success, 0 on error (type mismatch, OOM, or NULL params).
 */
int map_set(Value *vm, const char *key, Value v) {
  if (!vm || vm->type != VAL_MAP || !vm->map || !key) {
    free_value(v);
    return 0;
  }
  Map *m = (Map *)vm->map;
  for (int i = 0; i < m->count; ++i) {
    if (strcmp(m->keys[i], key) == 0) {
      free_value(m->vals[i]);
      m->vals[i] = v;
      return 1;
    }
  }
  if (!map_ensure_cap(m, m->count + 1)) {
    free_value(v);
    return 0;
  }
  m->keys[m->count] = strdup(key);
  m->vals[m->count] = v;
  m->count++;
  return 1;
}

/**
 * @brief Look up a key and copy the stored value into out.
 *
 * The returned value is a deep copy; caller owns it and must free it.
 *
 * @param vm  Source map Value (VAL_MAP).
 * @param key Key to search for.
 * @param out Output pointer to receive a copy; may be NULL to only test presence.
 * @return 1 if found (and out filled if non-NULL), 0 otherwise.
 */
int map_get_copy(const Value *vm, const char *key, Value *out) {
  if (!vm || vm->type != VAL_MAP || !vm->map || !key) return 0;
  Map *m = (Map *)vm->map;
  for (int i = 0; i < m->count; ++i) {
    if (strcmp(m->keys[i], key) == 0) {
      if (out) *out = copy_value(&m->vals[i]);
      return 1;
    }
  }
  return 0;
}

/**
 * @brief Check whether the map contains the specified key.
 * @param vm  Map Value (VAL_MAP).
 * @param key Key to search for.
 * @return 1 if present, 0 if absent or on invalid input.
 */
int map_has(const Value *vm, const char *key) {
  if (!vm || vm->type != VAL_MAP || !vm->map || !key) return 0;
  Map *m = (Map *)vm->map;
  for (int i = 0; i < m->count; ++i) {
    if (strcmp(m->keys[i], key) == 0) return 1;
  }
  return 0;
}

/**
 * @brief Return all map keys as an array of strings.
 *
 * Ownership: Caller must free the returned Value with free_value().
 *
 * @param vm Map Value (VAL_MAP).
 * @return Array Value of keys; empty array if vm is not a map or is empty.
 */
Value map_keys_array(const Value *vm) {
  if (!vm || vm->type != VAL_MAP || !vm->map) return make_array_from_values(NULL, 0);
  Map *m = (Map *)vm->map;
  if (m->count <= 0) return make_array_from_values(NULL, 0);
  Value *tmp = (Value *)malloc(sizeof(Value) * m->count);
  if (!tmp) return make_array_from_values(NULL, 0);
  for (int i = 0; i < m->count; ++i) {
    tmp[i] = make_string(m->keys[i]);
  }
  Value arr = make_array_from_values(tmp, m->count);
  for (int i = 0; i < m->count; ++i)
    free_value(tmp[i]);
  free(tmp);
  return arr;
}

/**
 * @brief Return all map values as an array (deep-copied).
 *
 * Ownership: Caller must free the returned Value with free_value().
 *
 * @param vm Map Value (VAL_MAP).
 * @return Array Value of values; empty array if vm is not a map or is empty.
 */
Value map_values_array(const Value *vm) {
  if (!vm || vm->type != VAL_MAP || !vm->map) return make_array_from_values(NULL, 0);
  Map *m = (Map *)vm->map;
  if (m->count <= 0) return make_array_from_values(NULL, 0);
  Value *tmp = (Value *)malloc(sizeof(Value) * m->count);
  if (!tmp) return make_array_from_values(NULL, 0);
  for (int i = 0; i < m->count; ++i) {
    tmp[i] = copy_value(&m->vals[i]);
  }
  Value arr = make_array_from_values(tmp, m->count);
  for (int i = 0; i < m->count; ++i)
    free_value(tmp[i]);
  free(tmp);
  return arr;
}
