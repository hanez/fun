/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
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
Value make_int(int64_t v);
Value make_bool(int v);
Value make_string(const char *s);
Value make_function(struct Bytecode *fn);
Value make_nil(void);
Value make_float(double v);

/* arrays */
Value make_array_from_values(const Value *vals, int count); /* deep-copies vals */
int array_length(const Value *v);                            /* returns -1 if not array */
int array_get_copy(const Value *v, int index, Value *out);   /* returns 0 on error; out = copy_value(item) */
int array_set(Value *v, int index, Value newElem);           /* returns 0 on error; takes ownership of newElem */
int array_push(Value *v, Value newElem);                     /* returns new length or -1 on error */
int array_pop(Value *v, Value *out);                         /* returns 1 on success, out takes ownership */
int array_insert(Value *v, int index, Value newElem);        /* returns new length or -1 */
int array_remove(Value *v, int index, Value *out);           /* returns 1 on success */
Value array_slice(const Value *v, int start, int end);       /* negative end means till end */
Value array_concat(const Value *a, const Value *b);          /* returns new array */

/* maps (string keys) */
Value make_map_empty(void);                                  /* new empty map */
int map_set(Value *m, const char *key, Value v);             /* 1 on ok (takes ownership of v) */
int map_get_copy(const Value *m, const char *key, Value *out);/* 1 on found, out=copy */
int map_has(const Value *m, const char *key);                /* 1/0 */
Value map_keys_array(const Value *m);                        /* array of strings */
Value map_values_array(const Value *m);                      /* array of values (copies) */

/* copy/free */
Value copy_value(const Value *v);    /* deep for strings, RC for arrays/maps, shallow fn */
Value deep_copy_value(const Value *v); /* deep copy including arrays/maps */
void free_value(Value v);            /* frees owned resources */

/* utilities */
void print_value(const Value *v);
int value_is_truthy(const Value *v);
int value_equals(const Value *a, const Value *b);  /* int/string equality */

/* stringify into a newly-allocated C string; caller must free */
char *value_to_string_alloc(const Value *v);

/* array utils */
int array_contains(const Value *arr, const Value *needle);    /* 1/0 */
int array_index_of(const Value *arr, const Value *needle);    /* idx or -1 */
void array_clear(Value *arr);                                  /* free elements, count=0 */

/* string helpers returning newly allocated C strings or arrays */
char *string_substr(const char *s, int start, int len);       /* clamps bounds */
int string_find(const char *hay, const char *needle);          /* index or -1 */
Value string_split_to_array(const char *s, const char *sep);   /* array of strings */
char *array_join_with_sep(const Value *arr, const char *sep);  /* join items as strings */

#endif
