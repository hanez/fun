#include "value.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct Array {
    int refcount;
    int count;
    Value *items; /* owns items; each item owned by array */
} Array;

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

Value make_array_from_values(const Value *vals, int count) {
    if (count < 0) count = 0;
    Array *arr = (Array*)malloc(sizeof(Array));
    if (!arr) {
        Value nil = make_nil();
        return nil;
    }
    arr->refcount = 1;
    arr->count = count;
    if (count > 0) {
        arr->items = (Value*)malloc(sizeof(Value) * count);
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
    v.arr = (struct Array*)arr;
    return v;
}

int array_length(const Value *v) {
    if (!v || v->type != VAL_ARRAY || !v->arr) return -1;
    const Array *a = (const Array*)v->arr;
    return a->count;
}

int array_get_copy(const Value *v, int index, Value *out) {
    if (!v || v->type != VAL_ARRAY || !v->arr) return 0;
    const Array *a = (const Array*)v->arr;
    if (index < 0 || index >= a->count) return 0;
    if (out) *out = copy_value(&a->items[index]);
    return 1;
}

int array_set(Value *v, int index, Value newElem) {
    if (!v || v->type != VAL_ARRAY || !v->arr) return 0;
    Array *a = (Array*)v->arr;
    if (index < 0 || index >= a->count) return 0;
    /* replace element: free old, take ownership of newElem */
    free_value(a->items[index]);
    a->items[index] = newElem;
    return 1;
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
        case VAL_ARRAY: {
            Array *a = (Array*)v->arr;
            out.arr = (struct Array*)a;
            if (a) a->refcount++;
            break;
        }
        case VAL_NIL:
        default:
            break;
    }
    return out;
}

void free_value(Value v) {
    if (v.type == VAL_STRING && v.s) {
        free(v.s);
    } else if (v.type == VAL_ARRAY && v.arr) {
        Array *a = (Array*)v.arr;
        if (--a->refcount == 0) {
            for (int i = 0; i < a->count; ++i) {
                free_value(a->items[i]);
            }
            free(a->items);
            free(a);
        }
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
        case VAL_ARRAY: {
            const Array *a = (const Array*)v->arr;
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
        case VAL_ARRAY: {
            const Array *a = (const Array*)v->arr;
            return a && a->count > 0;
        }
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
        case VAL_ARRAY: {
            int n = array_length(v);
            if (n < 0) n = 0;
            snprintf(buf, sizeof(buf), "[array n=%d]", n);
            return strdup(buf);
        }
        case VAL_NIL:
        default:
            return strdup("nil");
    }
}

