/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-11-25
 */

/* PCRE2_MATCH */
case OP_PCRE2_MATCH: {
#ifdef FUN_WITH_PCRE2
    Value vflags = pop_value(vm);
    Value vtext  = pop_value(vm);
    Value vpat   = pop_value(vm);
    int flags = 0;
    if (vflags.type == VAL_INT || vflags.type == VAL_BOOL) flags = (int)vflags.i;
    char *pattern = value_to_string_alloc(&vpat);
    char *subject = value_to_string_alloc(&vtext);
    free_value(vflags);
    free_value(vtext);
    free_value(vpat);
    if (!pattern || !subject) {
        if (pattern) free(pattern);
        if (subject) free(subject);
        push_value(vm, make_nil());
        break;
    }
    #ifndef PCRE2_CODE_UNIT_WIDTH
    #define PCRE2_CODE_UNIT_WIDTH 8
    #endif
    #include <pcre2.h>
    int errorcode; PCRE2_SIZE erroff;
    uint32_t opt = 0;
    if (flags & 1)  opt |= PCRE2_CASELESS;   /* I */
    if (flags & 2)  opt |= PCRE2_MULTILINE;  /* M */
    if (flags & 4)  opt |= PCRE2_DOTALL;     /* S */
    if (flags & 8)  opt |= PCRE2_UTF;        /* U */
    if (flags & 16) opt |= PCRE2_EXTENDED;   /* X */
    pcre2_code *re = pcre2_compile((PCRE2_SPTR)pattern, PCRE2_ZERO_TERMINATED, opt, &errorcode, &erroff, NULL);
    if (!re) {
        free(pattern); free(subject);
        push_value(vm, make_nil());
        break;
    }
    pcre2_match_data *mdata = pcre2_match_data_create_from_pattern(re, NULL);
    int rc = pcre2_match(re, (PCRE2_SPTR)subject, (PCRE2_SIZE)strlen(subject), 0, 0, mdata, NULL);
    if (rc <= 0) {
        pcre2_match_data_free(mdata);
        pcre2_code_free(re);
        free(pattern); free(subject);
        push_value(vm, make_nil());
        break;
    }
    PCRE2_SIZE *ov = pcre2_get_ovector_pointer(mdata);
    /* Build result map */
    Value res = make_map_empty();
    int start0 = (int)ov[0];
    int end0 = (int)ov[1];
    char *full = string_substr(subject, start0, end0 - start0);
    (void)map_set(&res, "full", make_string(full ? full : ""));
    if (full) free(full);
    (void)map_set(&res, "start", make_int(start0));
    (void)map_set(&res, "end", make_int(end0));
    /* groups array (excluding group 0) */
    Value groups = make_array_from_values(NULL, 0);
    for (int i = 1; i < rc; ++i) {
        int s = (int)ov[2*i];
        int e = (int)ov[2*i+1];
        char *gstr = (s >= 0 && e >= s) ? string_substr(subject, s, e - s) : NULL;
        Value gv = make_string(gstr ? gstr : "");
        if (gstr) free(gstr);
        (void)array_push(&groups, gv);
    }
    (void)map_set(&res, "groups", groups);
    pcre2_match_data_free(mdata);
    pcre2_code_free(re);
    free(pattern); free(subject);
    push_value(vm, res);
#else
    Value a = pop_value(vm); free_value(a);
    Value b = pop_value(vm); free_value(b);
    Value c = pop_value(vm); free_value(c);
    push_value(vm, make_nil());
#endif
    break;
}
