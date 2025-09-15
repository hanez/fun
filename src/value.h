#ifndef FUN_VALUE_H
#define FUN_VALUE_H

#include <inttypes.h>

struct Bytecode; /* forward */
struct Array;    /* forward */
struct Map;      /* forward */

typedef enum {
    VAL_INT,
    VAL_STRING,
    VAL_FUNCTION,
    VAL_ARRAY,
    VAL_MAP,
    VAL_NIL
} ValueType;

typedef struct {
    ValueType type;
    union {
        int64_t i;
        char *s;
        struct Bytecode *fn;
        struct Array *arr;
        struct Map *map;
    };
} Value;

/* constructors / helpers */
Value make_int(int64_t v);
Value make_string(const char *s);
Value make_function(struct Bytecode *fn);
Value make_nil(void);

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

