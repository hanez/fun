/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-11-24
 */

/* json-c helpers and VM opcode cases (included from vm.c) */

#include "value.h"
#include "vm.h"

#ifdef FUN_WITH_JSON
  #include <json-c/json.h>
  #include <string.h>
#endif

/* --- Conversion helpers between json-c and Fun Value --- */
#ifdef FUN_WITH_JSON
static Value json_to_fun(json_object *j) {
    if (!j) return make_nil();
    enum json_type t = json_object_get_type(j);
    switch (t) {
        case json_type_null: return make_nil();
        case json_type_boolean: return make_bool(json_object_get_boolean(j));
        case json_type_double: return make_float(json_object_get_double(j));
        case json_type_int: return make_int((int64_t)json_object_get_int64(j));
        case json_type_string: return make_string(json_object_get_string(j));
        case json_type_array: {
            size_t n = json_object_array_length(j);
            if (n == 0) {
                return make_array_from_values(NULL, 0);
            }
            Value *vals = (Value*)malloc(sizeof(Value) * n);
            if (!vals) return make_array_from_values(NULL, 0);
            for (size_t i = 0; i < n; ++i) {
                json_object *item = json_object_array_get_idx(j, (int)i);
                vals[i] = json_to_fun(item);
            }
            Value arr = make_array_from_values(vals, (int)n);
            for (size_t i = 0; i < n; ++i) free_value(vals[i]);
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

static json_object* fun_to_json(const Value *v) {
    switch (v->type) {
        case VAL_NIL: return json_object_new_null();
        case VAL_BOOL: return json_object_new_boolean(v->i ? 1 : 0);
        case VAL_INT: return json_object_new_int64(v->i);
        case VAL_FLOAT: return json_object_new_double(v->d);
        case VAL_STRING: return json_object_new_string(v->s ? v->s : "");
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
#endif /* FUN_WITH_JSON */

/* Note: The VM opcode case handlers are included from vm/vm switch via vm/json/ops.c */
