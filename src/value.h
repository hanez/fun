#ifndef FUN_VALUE_H
#define FUN_VALUE_H

#include <inttypes.h>

struct Bytecode; /* forward */

typedef enum {
    VAL_INT,
    VAL_STRING,
    VAL_FUNCTION,
    VAL_NIL
} ValueType;

typedef struct {
    ValueType type;
    union {
        int64_t i;
        char *s;
        struct Bytecode *fn;
    };
} Value;

/* constructors / helpers */
Value make_int(int64_t v);
Value make_string(const char *s);
Value make_function(struct Bytecode *fn);
Value make_nil(void);

/* copy (deep for strings), free (free string only) */
Value copy_value(const Value *v);
void free_value(Value v);

/* utilities */
void print_value(const Value *v);
int value_is_truthy(const Value *v);

#endif

