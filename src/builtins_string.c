/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the ISC license.
 * https://opensource.org/license/isc-license-txt
 */

#include "value.h"
#include <stdlib.h>

/* String built-ins wrappers used by VM opcodes */

Value bi_split(const Value *str, const Value *sep) {
    const char *s = (str && str->type == VAL_STRING && str->s) ? str->s : "";
    const char *p = (sep && sep->type == VAL_STRING && sep->s) ? sep->s : "";
    return string_split_to_array(s, p);
}

Value bi_join(const Value *arr, const Value *sep) {
    const char *p = (sep && sep->type == VAL_STRING && sep->s) ? sep->s : "";
    char *s = array_join_with_sep(arr, p);
    Value out = make_string(s ? s : "");
    if (s) free(s);
    return out;
}

Value bi_substr(const Value *str, int start, int len) {
    const char *s = (str && str->type == VAL_STRING && str->s) ? str->s : "";
    char *sub = string_substr(s, start, len);
    Value out = make_string(sub ? sub : "");
    if (sub) free(sub);
    return out;
}

int bi_find(const Value *hay, const Value *needle) {
    const char *h = (hay && hay->type == VAL_STRING && hay->s) ? hay->s : "";
    const char *n = (needle && needle->type == VAL_STRING && needle->s) ? needle->s : "";
    return string_find(h, n);
}
