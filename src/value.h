/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file value.h
 * @brief Defines the Value type and associated functions for the Fun VM.
 *
 * This file defines the `Value` type, which represents all possible data types
 * in the Fun language, including integers, strings, functions, arrays, maps, and nil.
 * It also provides utility functions for creating, copying, and freeing `Value` objects.
 *
 * Key Types:
 * - `ValueType`: Enumeration of supported value types (e.g., `VAL_INT`, `VAL_STRING`).
 * - `Value`: Union type that can hold any Fun value.
 *
 * Functions:
 * - `make_int`, `make_string`, `make_function`, `make_nil`: Constructors for `Value`.
 * - `array_length`, `array_get_copy`, `array_set`: Array manipulation functions.
 * - `make_map_empty`, `map_set`, `map_get_copy`: Map manipulation functions.
 * - `copy_value`, `deep_copy_value`, `free_value`: Functions for copying and freeing values.
 *
 * Example:
 * ```c
 * Value num = make_int(42);
 * Value str = make_string("Hello, Fun!");
 * ```
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

#ifndef FUN_VALUE_H
#define FUN_VALUE_H

#include <inttypes.h>

struct Bytecode; /* forward */
struct Array;    /* forward */
struct Map;      /* forward */

/**
 * @brief Enumeration of all runtime value types supported by Fun.
 */
typedef enum {
  VAL_INT,
  VAL_BOOL,
  VAL_STRING,
  VAL_FUNCTION,
  VAL_ARRAY,
  VAL_MAP,
  VAL_NIL,
  VAL_FLOAT
} ValueType;

/**
 * @brief Tagged union representing a Fun value.
 *
 * The active field in the anonymous union is determined by @ref ValueType in
 * the @c type tag. Ownership/RC semantics for complex members (arrays/maps)
 * are defined by the array/map APIs.
 */
typedef struct {
  ValueType type;
  union {
    int64_t i;
    double d;
    char *s;
    struct Bytecode *fn;
    struct Array *arr;
    struct Map *map;
  };
} Value;

/* constructors / helpers */
/** Create an integer Value. */
Value make_int(int64_t v);
/** Create a boolean Value (0=false, non-zero=true). */
Value make_bool(int v);
/** Create a string Value by copying @p s. */
Value make_string(const char *s);
/** Create a function Value from bytecode pointer (shallow). */
Value make_function(struct Bytecode *fn);
/** Create a nil Value. */
Value make_nil(void);
/** Create a floating-point Value. */
Value make_float(double v);

/* arrays */
/** Build an array from a list of Values (deep-copies vals). */
Value make_array_from_values(const Value *vals, int count);
/** Get array length or -1 if @p v is not an array. */
int array_length(const Value *v);
/** Copy array item at index to out; returns 0 on error. */
int array_get_copy(const Value *v, int index, Value *out);
/** Set element at index; takes ownership of newElem; 0 on error. */
int array_set(Value *v, int index, Value newElem);
/** Push new element; returns new length or -1 on error. */
int array_push(Value *v, Value newElem);
/** Pop last element into out; returns 1 on success. */
int array_pop(Value *v, Value *out);
/** Insert at index; returns new length or -1 on error. */
int array_insert(Value *v, int index, Value newElem);
/** Remove at index into out; returns 1 on success. */
int array_remove(Value *v, int index, Value *out);
/** Return slice [start,end) (negative end means till end). */
Value array_slice(const Value *v, int start, int end);
/** Concatenate arrays a and b into a new array. */
Value array_concat(const Value *a, const Value *b);

/* maps (string keys) */
/** Create a new empty string-keyed map Value. */
Value make_map_empty(void);
/** Set key to v (takes ownership); returns 1 on success. */
int map_set(Value *m, const char *key, Value v);
/** Lookup key; returns 1 and copies value to out on success. */
int map_get_copy(const Value *m, const char *key, Value *out);
/** Test if key exists; returns 1/0. */
int map_has(const Value *m, const char *key);
/** Return array of string keys. */
Value map_keys_array(const Value *m);
/** Return array of values (copies). */
Value map_values_array(const Value *m);

/* copy/free */
/** Shallow/deep copy depending on type (deep for strings, RC for arrays/maps). */
Value copy_value(const Value *v);
/** Deep copy including arrays/maps. */
Value deep_copy_value(const Value *v);
/** Free owned resources of v. */
void free_value(Value v);

/* utilities */
/** Print value in a human-readable form to stdout. */
void print_value(const Value *v);
/** Truthiness predicate used by the language semantics. */
int value_is_truthy(const Value *v);
/** Equality for ints/strings; other types may be pointer/semantic based. */
int value_equals(const Value *a, const Value *b);

/* stringify into a newly-allocated C string; caller must free */
/** Convert Value to a newly allocated C string; caller must free. */
char *value_to_string_alloc(const Value *v);

/* array utils */
/** Return 1 if needle equals any element in arr. */
int array_contains(const Value *arr, const Value *needle);
/** Return index of needle in arr or -1. */
int array_index_of(const Value *arr, const Value *needle);
/** Free elements and reset count to 0. */
void array_clear(Value *arr);

/* string helpers returning newly allocated C strings or arrays */
/** Create newly allocated substring (bounds clamped). */
char *string_substr(const char *s, int start, int len);
/** Find first index of needle in hay or -1. */
int string_find(const char *hay, const char *needle);
/** Split C string by sep into Value array of strings. */
Value string_split_to_array(const char *s, const char *sep);
/** Join Value array items into a newly allocated C string with separator. */
char *array_join_with_sep(const Value *arr, const char *sep);

#endif
