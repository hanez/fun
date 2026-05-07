/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file json.c
 * @brief json-c helpers for Fun VM JSON-related opcodes (conditional build).
 *
 * This module centralizes small utilities that convert between json-c objects
 * and the Fun VM's Value type. Keeping the concrete conversion logic in
 * src/extensions/ allows VM opcode implementations to remain minimal — they
 * focus on VM stack marshalling and delegate the heavy lifting here, mirroring
 * other extensions (PCRE2, SQLite, XML2, INI).
 *
 * Build-time feature flag:
 * - All code in this file is compiled only when FUN_WITH_JSON is enabled. When
 *   disabled, JSON-related opcodes should provide safe no-op fallbacks in
 *   their respective VM files.
 *
 * Type mapping between json-c and Fun:
 * - json null        -> Fun Nil
 * - json boolean     -> Fun Bool
 * - json number      -> Fun Int (64-bit) or Fun Float (double) depending on
 *                       the underlying json-c representation
 * - json string      -> Fun String (assumed UTF-8 from json-c)
 * - json array       -> Fun Array (elements converted recursively)
 * - json object      -> Fun Map<string, any> (values converted recursively)
 *
 * Ownership and memory:
 * - The conversion functions allocate new Fun Values and/or new json-c trees.
 *   The caller owns the returned result and is responsible for releasing it
 *   (free_value for Fun Values, json_object_put for json-c objects) when no
 *   longer needed.
 * - No global state is retained; all allocations are tied to the returned
 *   objects.
 *
 * Encoding and limits:
 * - json-c strings are treated as UTF-8; the Fun VM strings are expected to be
 *   UTF-8 as well. No transcoding is performed.
 * - Deeply nested inputs convert recursively; extremely deep trees may exhaust
 *   the C stack. Cycles are not possible in well-formed JSON; if presented via
 *   custom json_object graphs, behaviour is undefined (may loop or duplicate).
 *
 * Thread-safety:
 * - Not intrinsically thread-safe or unsafe. The helpers are stateless; each
 *   invocation operates on its arguments only. Coordinate external access to
 *   shared json_object instances if they are mutated concurrently.
 */

#ifdef FUN_WITH_JSON
//#include "value.h"
//#include "vm.h"

#include <json-c/json.h>
//#include <string.h>

/**
 * @brief Convert a json-c object tree into a Fun Value.
 *
 * Mapping rules:
 * - null -> Nil
 * - boolean -> Bool
 * - number (int/double) -> Int/Float (uses json_object_get_int64/json_object_get_double)
 * - string -> String (assumes UTF-8, no transcoding)
 * - array -> Array (elements converted recursively, preserving order)
 * - object -> Map<string, any> (values converted recursively; keys are copied as-is)
 *
 * Error handling and ownership:
 * - If j is NULL, returns Nil.
 * - The returned Value is newly created and owned by the caller, who must
 *   dispose it with free_value() when done. Internally allocated temporaries
 *   are released before returning.
 *
 * Notes:
 * - json-c may store numbers as double even when they look integral; such
 *   values will end up as Float in Fun.
 * - Deep or large JSON inputs are traversed recursively; excessive depth could
 *   lead to stack pressure.
 *
 * @param j Pointer to a json_object (may be NULL).
 * @return Value Converted Value (Nil on NULL input or on unsupported types).
 */
static Value json_to_fun(json_object *j) {
  if (!j) return make_nil();
  enum json_type t = json_object_get_type(j);
  switch (t) {
  case json_type_null:
    return make_nil();
  case json_type_boolean:
    return make_bool(json_object_get_boolean(j));
  case json_type_double:
    return make_float(json_object_get_double(j));
  case json_type_int:
    return make_int((int64_t)json_object_get_int64(j));
  case json_type_string:
    return make_string(json_object_get_string(j));
  case json_type_array: {
    size_t n = json_object_array_length(j);
    if (n == 0) {
      return make_array_from_values(NULL, 0);
    }
    Value *vals = (Value *)malloc(sizeof(Value) * n);
    if (!vals) return make_array_from_values(NULL, 0);
    for (size_t i = 0; i < n; ++i) {
      json_object *item = json_object_array_get_idx(j, (int)i);
      vals[i] = json_to_fun(item);
    }
    Value arr = make_array_from_values(vals, (int)n);
    for (size_t i = 0; i < n; ++i)
      free_value(vals[i]);
    free(vals);
    return arr;
  }
  case json_type_object: {
    Value map = make_map_empty();
    json_object_object_foreach(j, key, val) {
      (void)map_set(&map, key, json_to_fun(val));
    }
    return map;
  }
  default:
    return make_nil();
  }
}

/**
 * @brief Convert a Fun Value into a newly allocated json-c object tree.
 *
 * Mapping rules:
 * - Nil -> json null
 * - Bool -> json boolean
 * - Int (64-bit) -> json int64
 * - Float (double) -> json double
 * - String -> json string (assumes UTF-8 already)
 * - Array -> json array (elements converted recursively)
 * - Map   -> json object (keys are taken from the map's string keys)
 *
 * Unsupported or opaque Fun types are stringified using the placeholder
 * "<unsupported>" to keep the conversion total.
 *
 * Ownership:
 * - The caller owns the returned json_object* and must release it with
 *   json_object_put() when no longer needed.
 *
 * Notes and limitations:
 * - Map keys are enumerated via map_keys_array(); only string keys are used.
 *   Non-string keys are ignored.
 * - json-c does not support NaN/Inf as JSON numbers in a standard way; if such
 *   values appear in Fun Float, they are forwarded to json-c as-is.
 *
 * @param v Pointer to the source Value (must not be NULL).
 * @return json_object* Newly created tree representing v.
 */
static json_object *fun_to_json(const Value *v) {
  switch (v->type) {
  case VAL_NIL:
    return json_object_new_null();
  case VAL_BOOL:
    return json_object_new_boolean(v->i ? 1 : 0);
  case VAL_INT:
    return json_object_new_int64(v->i);
  case VAL_FLOAT:
    return json_object_new_double(v->d);
  case VAL_STRING:
    return json_object_new_string(v->s ? v->s : "");
  case VAL_ARRAY: {
    json_object *arr = json_object_new_array();
    int n = array_length(v);
    for (int i = 0; i < n; ++i) {
      Value item;
      if (array_get_copy(v, i, &item)) {
        json_object_array_add(arr, fun_to_json(&item));
        free_value(item);
      } else {
        json_object_array_add(arr, json_object_new_null());
      }
    }
    return arr;
  }
  case VAL_MAP: {
    json_object *obj = json_object_new_object();
    /* We don't have an iterator API; use keys() helper */
    Value keys = map_keys_array(v);
    int kn = array_length(&keys);
    for (int i = 0; i < kn; ++i) {
      Value k;
      if (!array_get_copy(&keys, i, &k)) continue;
      if (k.type == VAL_STRING && k.s) {
        Value val;
        if (map_get_copy(v, k.s, &val)) {
          json_object_object_add(obj, k.s, fun_to_json(&val));
          free_value(val);
        } else {
          json_object_object_add(obj, k.s, json_object_new_null());
        }
      }
      free_value(k);
    }
    free_value(keys);
    return obj;
  }
  default:
    /* Fallback: stringify unsupported types */
    return json_object_new_string("<unsupported>");
  }
}
#endif
