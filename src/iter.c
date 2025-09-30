/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

#include "value.h"
#include <stdlib.h>

/* enumerate(arr) -> [[0, v0], [1, v1], ...] */
Value bi_enumerate(const Value *arr) {
    int n = array_length(arr);
    if (n <= 0) return make_array_from_values(NULL, 0);
    Value *pairs = (Value*)malloc(sizeof(Value) * n);
    if (!pairs) return make_array_from_values(NULL, 0);
    for (int i = 0; i < n; ++i) {
        Value elem;
        array_get_copy(arr, i, &elem);
        Value kv_vals[2];
        kv_vals[0] = make_int(i);
        kv_vals[1] = elem;
        Value kv = make_array_from_values(kv_vals, 2);
        free_value(kv_vals[0]);
        free_value(kv_vals[1]);
        pairs[i] = kv;
    }
    Value out = make_array_from_values(pairs, n);
    for (int i = 0; i < n; ++i) free_value(pairs[i]);
    free(pairs);
    return out;
}

/* zip(a, b) -> [[a0,b0], [a1,b1], ...] up to min(len(a),len(b)) */
Value bi_zip(const Value *a, const Value *b) {
    int na = array_length(a);
    int nb = array_length(b);
    int n = na < nb ? na : nb;
    if (n <= 0) return make_array_from_values(NULL, 0);
    Value *pairs = (Value*)malloc(sizeof(Value) * n);
    if (!pairs) return make_array_from_values(NULL, 0);
    for (int i = 0; i < n; ++i) {
        Value av, bv;
        array_get_copy(a, i, &av);
        array_get_copy(b, i, &bv);
        Value kv_vals[2];
        kv_vals[0] = av;
        kv_vals[1] = bv;
        Value kv = make_array_from_values(kv_vals, 2);
        free_value(kv_vals[0]);
        free_value(kv_vals[1]);
        pairs[i] = kv;
    }
    Value out = make_array_from_values(pairs, n);
    for (int i = 0; i < n; ++i) free_value(pairs[i]);
    free(pairs);
    return out;
}
