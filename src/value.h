#ifndef FUN_VALUE_H
#define FUN_VALUE_H

#include <inttypes.h>

struct Bytecode; /* forward */
struct Array;    /* forward */

typedef enum {
    VAL_INT,
    VAL_STRING,
    VAL_FUNCTION,
    VAL_ARRAY,
    VAL_NIL
} ValueType;

typedef struct {
    ValueType type;
    union {
        int64_t i;
        char *s;
        struct Bytecode *fn;
        struct Array *arr;
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

/* copy/free */
Value copy_value(const Value *v);    /* deep for strings, RC for arrays, shallow fn */
void free_value(Value v);            /* frees owned resources */

/* utilities */
void print_value(const Value *v);
int value_is_truthy(const Value *v);

/* stringify into a newly-allocated C string; caller must free */
char *value_to_string_alloc(const Value *v);

#endif

