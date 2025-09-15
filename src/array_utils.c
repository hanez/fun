#include "value.h"
#include <string.h>

/* array utilities */

int array_contains(const Value *v, const Value *needle) {
    if (!v || v->type != VAL_ARRAY || !v->arr) return 0;
    int n = array_length(v);
    for (int i = 0; i < n; ++i) {
        Value item;
        if (array_get_copy(v, i, &item)) {
            int eq = value_equals(&item, needle);
            free_value(item);
            if (eq) return 1;
        }
    }
    return 0;
}

int array_index_of(const Value *v, const Value *needle) {
    if (!v || v->type != VAL_ARRAY || !v->arr) return -1;
    int n = array_length(v);
    for (int i = 0; i < n; ++i) {
        Value item;
        if (array_get_copy(v, i, &item)) {
            int eq = value_equals(&item, needle);
            free_value(item);
            if (eq) return i;
        }
    }
    return -1;
}

void array_clear(Value *v) {
    if (!v || v->type != VAL_ARRAY || !v->arr) return;
    int n = array_length(v);
    for (int i = 0; i < n; ++i) {
        Value item;
        if (array_get_copy(v, i, &item)) {
            free_value(item);
        }
    }
    /* internal clear uses public API to reset to zero length */
    /* Since we don't expose capacity, emulate by popping elements */
    Value out;
    while (array_pop(v, &out)) {
        free_value(out);
    }
}
