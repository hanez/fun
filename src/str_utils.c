/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

#include "value.h"
#include <stdlib.h>
#include <string.h>

/* string helpers returning newly allocated C strings or arrays */

char *string_substr(const char *s, int start, int len) {
    if (!s) return strdup("");
    int n = (int)strlen(s);
    if (start < 0) start = 0;
    if (start > n) start = n;
    if (len < 0) len = 0;
    if (start + len > n) len = n - start;
    char *out = (char*)malloc((size_t)len + 1);
    if (!out) return strdup("");
    memcpy(out, s + start, (size_t)len);
    out[len] = '\0';
    return out;
}

int string_find(const char *hay, const char *needle) {
    if (!hay || !needle) return -1;
    const char *p = strstr(hay, needle);
    if (!p) return -1;
    return (int)(p - hay);
}

Value string_split_to_array(const char *s, const char *sep) {
    if (!s) s = "";
    if (!sep) sep = "";
    int seplen = (int)strlen(sep);
    if (seplen == 0) {
        /* split into characters */
        int n = (int)strlen(s);
        if (n <= 0) return make_array_from_values(NULL, 0);
        Value *tmp = (Value*)malloc(sizeof(Value) * n);
        if (!tmp) return make_array_from_values(NULL, 0);
        for (int i = 0; i < n; ++i) {
            char ch[2] = { s[i], 0 };
            tmp[i] = make_string(ch);
        }
        Value arr = make_array_from_values(tmp, n);
        for (int i = 0; i < n; ++i) free_value(tmp[i]);
        free(tmp);
        return arr;
    }
    /* split by separator */
    Value *parts = NULL;
    int count = 0;
    int cap = 0;
    const char *cur = s;
    const char *pos = NULL;
    while ((pos = strstr(cur, sep)) != NULL) {
        int len = (int)(pos - cur);
        char *piece = (char*)malloc((size_t)len + 1);
        if (!piece) break;
        memcpy(piece, cur, (size_t)len);
        piece[len] = '\0';
        if (count >= cap) {
            cap = cap == 0 ? 4 : cap * 2;
            parts = (Value*)realloc(parts, sizeof(Value) * cap);
        }
        parts[count++] = make_string(piece);
        free(piece);
        cur = pos + seplen;
    }
    /* tail */
    char *tail = strdup(cur ? cur : "");
    if (count >= cap) {
        cap = cap == 0 ? 1 : cap + 1;
        parts = (Value*)realloc(parts, sizeof(Value) * cap);
    }
    parts[count++] = make_string(tail ? tail : "");
    free(tail);

    Value arr = make_array_from_values(parts, count);
    for (int i = 0; i < count; ++i) free_value(parts[i]);
    free(parts);
    return arr;
}

char *array_join_with_sep(const Value *v, const char *sep) {
    if (!v || v->type != VAL_ARRAY || !v->arr) return strdup("");
    if (!sep) sep = "";
    /* Array is defined in value.c; we only need safe public access. */
    const int n = array_length(v);
    if (n <= 0) return strdup("");

    char **parts = (char**)malloc(sizeof(char*) * n);
    if (!parts) return strdup("");

    size_t total = 0;
    for (int i = 0; i < n; ++i) {
        Value item;
        if (!array_get_copy(v, i, &item)) {
            parts[i] = strdup("");
        } else {
            parts[i] = value_to_string_alloc(&item);
            free_value(item);
        }
        total += strlen(parts[i]);
        if (i + 1 < n) total += strlen(sep);
    }

    char *out = (char*)malloc(total + 1);
    if (!out) {
        for (int i = 0; i < n; ++i) free(parts[i]);
        free(parts);
        return strdup("");
    }

    size_t off = 0;
    for (int i = 0; i < n; ++i) {
        size_t li = strlen(parts[i]);
        memcpy(out + off, parts[i], li); off += li;
        if (i + 1 < n) {
            size_t ls = strlen(sep);
            memcpy(out + off, sep, ls); off += ls;
        }
        free(parts[i]);
    }
    free(parts);
    out[off] = '\0';
    return out;
}
