#include "value.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Value make_int(int64_t v) {
    Value val;
    val.type = VAL_INT;
    val.i = v;
    return val;
}

Value make_string(const char *s) {
    Value val;
    val.type = VAL_STRING;
    if (s) val.s = strdup(s);
    else val.s = strdup("");
    return val;
}

Value make_function(struct Bytecode *fn) {
    Value val;
    val.type = VAL_FUNCTION;
    val.fn = fn;
    return val;
}

Value make_nil(void) {
    Value v;
    v.type = VAL_NIL;
    return v;
}

Value copy_value(const Value *v) {
    Value out;
    out.type = v->type;
    switch (v->type) {
        case VAL_INT:
            out.i = v->i;
            break;
        case VAL_STRING:
            out.s = v->s ? strdup(v->s) : strdup("");
            break;
        case VAL_FUNCTION:
            out.fn = v->fn; /* shallow copy pointer */
            break;
        case VAL_NIL:
        default:
            break;
    }
    return out;
}

void free_value(Value v) {
    if (v.type == VAL_STRING && v.s) {
        free(v.s);
    }
    /* VAL_FUNCTION: we *do not* free the Bytecode here (caller frees it) */
}

void print_value(const Value *v) {
    switch (v->type) {
        case VAL_INT:
            printf("%" PRId64, v->i);
            break;
        case VAL_STRING:
            printf("%s", v->s ? v->s : "");
            break;
        case VAL_FUNCTION:
            printf("<function@%p>", (void*)v->fn);
            break;
        case VAL_NIL:
        default:
            printf("nil");
            break;
    }
}

int value_is_truthy(const Value *v) {
    switch (v->type) {
        case VAL_INT:
            return v->i != 0;
        case VAL_STRING:
            return v->s && v->s[0] != '\0';
        case VAL_FUNCTION:
            return 1;
        case VAL_NIL:
        default:
            return 0;
    }
}

/* allocate a printable C string for the value; caller must free */
char *value_to_string_alloc(const Value *v) {
    if (!v) return strdup("nil");
    char buf[128];
    switch (v->type) {
        case VAL_INT: {
            char tmp[64];
            snprintf(tmp, sizeof(tmp), "%" PRId64, v->i);
            return strdup(tmp);
        }
        case VAL_STRING:
            return strdup(v->s ? v->s : "");
        case VAL_FUNCTION: {
            snprintf(buf, sizeof(buf), "<function@%p>", (void*)v->fn);
            return strdup(buf);
        }
        case VAL_NIL:
        default:
            return strdup("nil");
    }
}

