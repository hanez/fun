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

/* PCRE2_TEST */
case OP_PCRE2_TEST: {
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
        push_value(vm, make_int(0));
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
        push_value(vm, make_int(0));
        break;
    }
    pcre2_match_data *mdata = pcre2_match_data_create_from_pattern(re, NULL);
    int rc = pcre2_match(re, (PCRE2_SPTR)subject, (PCRE2_SIZE)strlen(subject), 0, 0, mdata, NULL);
    pcre2_match_data_free(mdata);
    pcre2_code_free(re);
    free(pattern); free(subject);
    push_value(vm, make_int(rc >= 0 ? 1 : 0));
#else
    /* pop args and return 0 when PCRE2 disabled */
    Value a = pop_value(vm); free_value(a);
    Value b = pop_value(vm); free_value(b);
    Value c = pop_value(vm); free_value(c);
    push_value(vm, make_int(0));
#endif
    break;
}
