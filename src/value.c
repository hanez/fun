/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file value.c
 * @brief Implementation of the runtime Value type, including constructors,
 *        dynamic array/map utilities, copying, comparison, printing, and
 *        string conversion helpers.
 *
 * This translation unit provides the concrete operations for the Fun
 * programming language's Value structure (ints, floats, bools, strings,
 * arrays, maps, functions and nil). It is used by the VM and standard
 * library to construct and manipulate runtime values.
 */

#include "value.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Compile helper implementations into this TU to avoid build system changes */
#include "array_utils.c"
#include "str_utils.c"

typedef struct Array {
  int refcount;
  int count;
  Value *items; /* owns items; each item owned by array */
} Array;

typedef struct Map {
  int refcount;
  int count;
  int cap;
  char **keys; /* each key owned here */
  Value *vals; /* each value owned here */
} Map;

/**
 * @brief Construct a Value representing a 64-bit integer.
 *
 * @param v The integer payload.
 * @return A Value with type VAL_INT holding v.
 */
Value make_int(int64_t v) {
  Value val;
  val.type = VAL_INT;
  val.i = v;
  return val;
}

/**
 * @brief Construct a Value representing a double-precision float.
 *
 * @param v The floating-point payload.
 * @return A Value with type VAL_FLOAT holding v.
 */
Value make_float(double v) {
  Value val;
  val.type = VAL_FLOAT;
  val.d = v;
  return val;
}

/**
 * @brief Construct a boolean Value.
 *
 * Any non-zero input is treated as true, zero as false.
 *
 * @param v Integer truthy/falsey indicator.
 * @return A Value with type VAL_BOOL and normalized 0/1 payload.
 */
Value make_bool(int v) {
  Value val;
  val.type = VAL_BOOL;
  val.i = v ? 1 : 0;
  return val;
}

/**
 * @brief Construct a string Value by duplicating the given C string.
 *
 * If s is NULL, an empty string is used. The returned Value owns an allocated
 * copy which must be released via free_value.
 *
 * @param s NUL-terminated C string (may be NULL).
 * @return A Value with type VAL_STRING.
 */
Value make_string(const char *s) {
  Value val;
  val.type = VAL_STRING;
  if (s)
    val.s = strdup(s);
  else
    val.s = strdup("");
  return val;
}

/**
 * @brief Construct a function Value referencing bytecode.
 *
 * The Bytecode pointer is stored as-is; ownership/lifetime is managed by the
 * caller/VM and not freed by free_value.
 *
 * @param fn Pointer to function bytecode (may be NULL to represent an invalid function).
 * @return A Value with type VAL_FUNCTION.
 */
Value make_function(struct Bytecode *fn) {
  Value val;
  val.type = VAL_FUNCTION;
  val.fn = fn;
  return val;
}

/**
 * @brief Construct a nil Value.
 *
 * @return A Value with type VAL_NIL.
 */
Value make_nil(void) {
  Value v;
  v.type = VAL_NIL;
  return v;
}

/**
 * @brief Create an array Value by copying items from an input span.
 *
 * Performs a shallow copy for scalars and reference-counted copy for arrays/maps
 * via copy_value. On allocation failure, returns VAL_NIL.
 *
 * @param vals Pointer to input items; may be NULL when count == 0.
 * @param count Number of items to copy (negative treated as 0).
 * @return A Value with type VAL_ARRAY or VAL_NIL on failure.
 */
Value make_array_from_values(const Value *vals, int count) {
  if (count < 0) count = 0;
  Array *arr = (Array *)malloc(sizeof(Array));
  if (!arr) {
    Value nil = make_nil();
    return nil;
  }
  arr->refcount = 1;
  arr->count = count;
  if (count > 0) {
    arr->items = (Value *)malloc(sizeof(Value) * count);
    if (!arr->items) {
      free(arr);
      Value nil = make_nil();
      return nil;
    }
    for (int i = 0; i < count; ++i) {
      arr->items[i] = copy_value(&vals[i]);
    }
  } else {
    arr->items = NULL;
  }
  Value v;
  v.type = VAL_ARRAY;
  v.arr = (struct Array *)arr;
  return v;
}

/**
 * @brief Get the element count of an array Value.
 *
 * @param v Array Value.
 * @return Number of elements, or -1 if v is not a valid array.
 */
int array_length(const Value *v) {
  if (!v || v->type != VAL_ARRAY || !v->arr) return -1;
  const Array *a = (const Array *)v->arr;
  return a->count;
}

/**
 * @brief Copy an array element into out.
 *
 * The element is copied with copy_value; ownership of out remains with caller.
 *
 * @param v Array Value.
 * @param index Zero-based index.
 * @param out Destination pointer to receive the copied Value (may be NULL to only validate index).
 * @return 1 on success, 0 on bounds/type error.
 */
int array_get_copy(const Value *v, int index, Value *out) {
  if (!v || v->type != VAL_ARRAY || !v->arr) return 0;
  const Array *a = (const Array *)v->arr;
  if (index < 0 || index >= a->count) return 0;
  if (out) *out = copy_value(&a->items[index]);
  return 1;
}

/**
 * @brief Replace an element of an array with a new Value.
 *
 * Takes ownership of newElem and frees the old element.
 *
 * @param v Array Value to mutate.
 * @param index Zero-based index to replace.
 * @param newElem New element (ownership transferred to array).
 * @return 1 on success, 0 on bounds/type error.
 */
int array_set(Value *v, int index, Value newElem) {
  if (!v || v->type != VAL_ARRAY || !v->arr) return 0;
  Array *a = (Array *)v->arr;
  if (index < 0 || index >= a->count) return 0;
  free_value(a->items[index]);
  a->items[index] = newElem; /* take ownership */
  return 1;
}

/**
 * @brief Ensure the internal items buffer can hold at least newCount items.
 *
 * May grow the allocation exponentially; initializes new slots to nil.
 *
 * @param a Internal Array pointer.
 * @param newCount Required minimum logical capacity.
 * @return 1 on success, 0 on allocation failure.
 */
static int ensure_array_capacity(Array *a, int newCount) {
  if (newCount <= a->count) return 1;
  /* grow to at least newCount; double strategy */
  int curr = a->count;
  int cap = curr;
  if (cap < 4) cap = 4;
  while (cap < newCount)
    cap *= 2;
  Value *newItems = (Value *)realloc(a->items, sizeof(Value) * cap);
  if (!newItems) return 0;
  /* if growing beyond current count, initialize new slots to nil */
  if (cap > a->count) {
    for (int i = a->count; i < cap; ++i) {
      newItems[i] = make_nil();
    }
  }
  a->items = newItems;
  return 1;
}

/**
 * @brief Append a Value to an array.
 *
 * On success, ownership of newElem is transferred to the array.
 *
 * @param v Array Value to append to.
 * @param newElem Element to append.
 * @return New array length on success (>=0), or -1 on failure/type error.
 */
int array_push(Value *v, Value newElem) {
  if (!v || v->type != VAL_ARRAY || !v->arr) return -1;
  Array *a = (Array *)v->arr;
  /* ensure capacity for count+1 by reallocating items array to at least count+1 elements */
  Value *newItems = (Value *)realloc(a->items, sizeof(Value) * (a->count + 1));
  if (!newItems) {
    free_value(newElem);
    return -1;
  }
  a->items = newItems;
  a->items[a->count] = newElem; /* take ownership */
  a->count += 1;
  return a->count;
}

/**
 * @brief Remove the last element from an array.
 *
 * If out is provided, ownership of the removed element is transferred to *out;
 * otherwise the element is freed.
 *
 * @param v Array Value to pop from.
 * @param out Optional destination for removed element.
 * @return 1 on success, 0 if array empty or invalid.
 */
int array_pop(Value *v, Value *out) {
  if (!v || v->type != VAL_ARRAY || !v->arr) return 0;
  Array *a = (Array *)v->arr;
  if (a->count <= 0) return 0;
  int idx = a->count - 1;
  if (out)
    *out = a->items[idx]; /* transfer ownership */
  else
    free_value(a->items[idx]);
  a->count -= 1;
  return 1;
}

/**
 * @brief Insert a new element at a specific position in an array.
 *
 * Index is clamped into [0, count]. Takes ownership of newElem.
 *
 * @param v Array Value to modify.
 * @param index Insertion index.
 * @param newElem Element to insert.
 * @return New array length on success (>=0), or -1 on allocation/type error.
 */
int array_insert(Value *v, int index, Value newElem) {
  if (!v || v->type != VAL_ARRAY || !v->arr) return -1;
  Array *a = (Array *)v->arr;
  if (index < 0) index = 0;
  if (index > a->count) index = a->count;
  Value *newItems = (Value *)realloc(a->items, sizeof(Value) * (a->count + 1));
  if (!newItems) {
    free_value(newElem);
    return -1;
  }
  a->items = newItems;
  /* shift right */
  for (int i = a->count; i > index; --i) {
    a->items[i] = a->items[i - 1];
  }
  a->items[index] = newElem; /* take ownership */
  a->count += 1;
  return a->count;
}

/**
 * @brief Remove an element at index from an array.
 *
 * If out is provided, ownership of the removed element is transferred; else it
 * is freed. Remaining items are shifted left.
 *
 * @param v Array Value to modify.
 * @param index Zero-based index to remove.
 * @param out Optional destination for removed element.
 * @return 1 on success, 0 on bounds/type error.
 */
int array_remove(Value *v, int index, Value *out) {
  if (!v || v->type != VAL_ARRAY || !v->arr) return 0;
  Array *a = (Array *)v->arr;
  if (index < 0 || index >= a->count) return 0;
  if (out)
    *out = a->items[index]; /* transfer ownership */
  else
    free_value(a->items[index]);
  /* shift left */
  for (int i = index; i < a->count - 1; ++i) {
    a->items[i] = a->items[i + 1];
  }
  a->count -= 1;
  return 1;
}

/**
 * @brief Create a shallow-copied slice of an array Value.
 *
 * Start and end are clamped into valid bounds; end < start yields empty array.
 *
 * @param v Source array Value.
 * @param start Inclusive zero-based start index (clamped to >= 0).
 * @param end Exclusive end index (clamped to <= length; -1 means length).
 * @return A new array Value (possibly empty) or VAL_NIL if v is not an array.
 */
Value array_slice(const Value *v, int start, int end) {
  if (!v || v->type != VAL_ARRAY || !v->arr) return make_nil();
  const Array *a = (const Array *)v->arr;
  int n = a->count;
  if (start < 0) start = 0;
  if (end < 0 || end > n) end = n;
  if (start > end) start = end;
  int m = end - start;
  if (m <= 0) {
    return make_array_from_values(NULL, 0);
  }
  return make_array_from_values(a->items + start, m);
}

/**
 * @brief Concatenate two array Values.
 *
 * Copies elements into a new array. If either input is not an array, returns
 * VAL_NIL.
 *
 * @param av First array.
 * @param bv Second array.
 * @return A new concatenated array Value or VAL_NIL on type/alloc error.
 */
Value array_concat(const Value *av, const Value *bv) {
  if (!av || !bv || av->type != VAL_ARRAY || bv->type != VAL_ARRAY) return make_nil();
  const Array *a = (const Array *)av->arr;
  const Array *b = (const Array *)bv->arr;
  int na = a ? a->count : 0;
  int nb = b ? b->count : 0;
  int total = na + nb;
  if (total <= 0) return make_array_from_values(NULL, 0);
  Value *tmp = (Value *)malloc(sizeof(Value) * total);
  if (!tmp) return make_nil();
  for (int i = 0; i < na; ++i)
    tmp[i] = a->items[i];
  for (int j = 0; j < nb; ++j)
    tmp[na + j] = b->items[j];
  Value out = make_array_from_values(tmp, total);
  /* free temporaries we copied from (deep copy in make_array_from_values) */
  free(tmp);
  return out;
}

/**
 * @brief Shallow copy a Value.
 *
 * Strings are duplicated, arrays/maps have their refcount incremented, and
 * function pointers are copied as-is.
 *
 * @param v Source Value.
 * @return A new Value with appropriate copy semantics.
 */
Value copy_value(const Value *v) {
  Value out;
  out.type = v->type;
  switch (v->type) {
  case VAL_INT:
    out.i = v->i;
    break;
  case VAL_FLOAT:
    out.d = v->d;
    break;
  case VAL_BOOL:
    out.i = v->i ? 1 : 0;
    break;
  case VAL_STRING:
    out.s = v->s ? strdup(v->s) : strdup("");
    break;
  case VAL_FUNCTION:
    out.fn = v->fn; /* shallow copy pointer */
    break;
  case VAL_ARRAY: {
    Array *a = (Array *)v->arr;
    out.arr = (struct Array *)a;
    if (a) a->refcount++;
    break;
  }
  case VAL_MAP: {
    Map *m = (Map *)v->map;
    out.map = (struct Map *)m;
    if (m) m->refcount++;
    break;
  }
  case VAL_NIL:
  default:
    break;
  }
  return out;
}

/* deep copy including arrays (recursively copies items) */
/**
 * @brief Deep copy a Value, recursively copying arrays and maps.
 *
 * Function Values are copied shallowly. On allocation failure, returns nil or
 * an empty container as appropriate.
 *
 * @param v Source Value.
 * @return A deep-copied Value.
 */
Value deep_copy_value(const Value *v) {
  switch (v->type) {
  case VAL_INT:
    return make_int(v->i);
  case VAL_FLOAT:
    return make_float(v->d);
  case VAL_BOOL:
    return make_bool(v->i);
  case VAL_STRING:
    return make_string(v->s ? v->s : "");
  case VAL_FUNCTION:
    return make_function(v->fn); /* shallow pointer for function bytecode */
  case VAL_ARRAY: {
    const Array *a = (const Array *)v->arr;
    if (!a || a->count <= 0) {
      return make_array_from_values(NULL, 0);
    }
    /* copy items deeply */
    Value *tmp = (Value *)malloc(sizeof(Value) * a->count);
    if (!tmp) return make_nil();
    for (int i = 0; i < a->count; ++i) {
      tmp[i] = deep_copy_value(&a->items[i]);
    }
    Value out = make_array_from_values(tmp, a->count);
    for (int i = 0; i < a->count; ++i) {
      free_value(tmp[i]);
    }
    free(tmp);
    return out;
  }
  case VAL_MAP: {
    const Map *m = (const Map *)v->map;
    if (!m || m->count <= 0) return make_map_empty();
    Value out = make_map_empty();
    for (int i = 0; i < m->count; ++i) {
      Value dv = deep_copy_value(&m->vals[i]);
      map_set(&out, m->keys[i], dv);
    }
    return out;
  }
  case VAL_NIL:
  default:
    return make_nil();
  }
}

/**
 * @brief Free dynamic storage owned by a Value.
 *
 * Strings are freed, arrays/maps are reference-counted and freed recursively
 * when their refcount drops to zero. Functions are not freed here.
 *
 * @param v Value whose owned resources should be released.
 */
void free_value(Value v) {
  if (v.type == VAL_STRING && v.s) {
    free(v.s);
  } else if (v.type == VAL_ARRAY && v.arr) {
    Array *a = (Array *)v.arr;
    if (--a->refcount == 0) {
      for (int i = 0; i < a->count; ++i) {
        free_value(a->items[i]);
      }
      free(a->items);
      free(a);
    }
  } else if (v.type == VAL_MAP && v.map) {
    Map *m = (Map *)v.map;
    if (--m->refcount == 0) {
      for (int i = 0; i < m->count; ++i) {
        if (m->keys[i]) free(m->keys[i]);
        free_value(m->vals[i]);
      }
      free(m->keys);
      free(m->vals);
      free(m);
    }
  }
  /* VAL_FUNCTION: we *do not* free the Bytecode here (caller frees it) */
}

/**
 * @brief Print a human-readable representation of a Value to stdout.
 *
 * Numbers are printed in decimal; arrays/maps are formatted compactly; strings
 * are printed without quotes.
 *
 * @param v Value to print.
 */
void print_value(const Value *v) {
  switch (v->type) {
  case VAL_INT:
    printf("%" PRId64, v->i);
    break;
  case VAL_FLOAT:
    printf("%.17g", v->d);
    break;
  case VAL_STRING:
    printf("%s", v->s ? v->s : "");
    break;
  case VAL_BOOL:
    printf("%s", v->i ? "true" : "false");
    break;
  case VAL_FUNCTION:
    printf("<function@%p>", (void *)v->fn);
    break;
  case VAL_ARRAY: {
    const Array *a = (const Array *)v->arr;
    printf("[");
    if (a) {
      for (int i = 0; i < a->count; ++i) {
        if (i > 0) printf(", ");
        print_value(&a->items[i]);
      }
    }
    printf("]");
    break;
  }
  case VAL_MAP: {
    const Map *m = (const Map *)v->map;
    printf("{");
    if (m) {
      for (int i = 0; i < m->count; ++i) {
        if (i > 0) printf(", ");
        printf("\"%s\": ", m->keys[i] ? m->keys[i] : "");
        print_value(&m->vals[i]);
      }
    }
    printf("}");
    break;
  }
  case VAL_NIL:
  default:
    printf("nil");
    break;
  }
}

/**
 * @brief Evaluate a Value's truthiness according to Fun language rules.
 *
 * Empty strings, zero numbers, nil and empty arrays are falsey; everything
 * else is truthy.
 *
 * @param v Value to evaluate.
 * @return 1 if truthy, 0 otherwise.
 */
int value_is_truthy(const Value *v) {
  switch (v->type) {
  case VAL_INT:
    return v->i != 0;
  case VAL_FLOAT:
    return v->d != 0.0;
  case VAL_BOOL:
    return v->i != 0;
  case VAL_STRING:
    return v->s && v->s[0] != '\0';
  case VAL_FUNCTION:
    return 1;
  case VAL_ARRAY: {
    const Array *a = (const Array *)v->arr;
    return a && a->count > 0;
  }
  case VAL_NIL:
  default:
    return 0;
  }
}

/* allocate a printable C string for the value; caller must free */
/**
 * @brief Allocate a printable C string for a Value.
 *
 * The returned string must be freed by the caller with free().
 *
 * @param v Value to convert.
 * @return Newly allocated NUL-terminated string describing v.
 */
char *value_to_string_alloc(const Value *v) {
  if (!v) return strdup("nil");
  char buf[128];
  switch (v->type) {
  case VAL_INT: {
    char tmp[64];
    snprintf(tmp, sizeof(tmp), "%" PRId64, v->i);
    return strdup(tmp);
  }
  case VAL_FLOAT: {
    char tmp[64];
    snprintf(tmp, sizeof(tmp), "%.17g", v->d);
    return strdup(tmp);
  }
  case VAL_STRING:
    return strdup(v->s ? v->s : "");
  case VAL_BOOL:
    return strdup(v->i ? "true" : "false");
  case VAL_FUNCTION: {
    snprintf(buf, sizeof(buf), "<function@%p>", (void *)v->fn);
    return strdup(buf);
  }
  case VAL_ARRAY: {
    int n = array_length(v);
    if (n < 0) n = 0;
    snprintf(buf, sizeof(buf), "[array n=%d]", n);
    return strdup(buf);
  }
  case VAL_MAP: {
    int n = 0;
    if (v->type == VAL_MAP && v->map) {
      const Map *m = (const Map *)v->map;
      n = m ? m->count : 0;
    }
    snprintf(buf, sizeof(buf), "{map n=%d}", n);
    return strdup(buf);
  }
  case VAL_NIL:
  default:
    return strdup("nil");
  }
}

/**
 * @brief Compare two Values for equality.
 *
 * Supports numeric cross-type equality between ints and floats. Strings are
 * compared by content. Other types default to pointer/type equality as
 * implemented in the switch.
 *
 * @param a First Value.
 * @param b Second Value.
 * @return 1 if equal, 0 otherwise.
 */
int value_equals(const Value *a, const Value *b) {
  // Numeric cross-type equality: int vs float compares numerically
  if ((a->type == VAL_INT || a->type == VAL_FLOAT) && (b->type == VAL_INT || b->type == VAL_FLOAT)) {
    double da = (a->type == VAL_INT) ? (double)a->i : a->d;
    double db = (b->type == VAL_INT) ? (double)b->i : b->d;
    return da == db;
  }
  if (a->type != b->type) return 0;
  switch (a->type) {
  case VAL_INT:
    return a->i == b->i;
  case VAL_BOOL:
    return (a->i != 0) == (b->i != 0);
  case VAL_STRING: {
    const char *sa = a->s ? a->s : "";
    const char *sb = b->s ? b->s : "";
    return strcmp(sa, sb) == 0;
  }
  default:
    return 0;
  }
}
