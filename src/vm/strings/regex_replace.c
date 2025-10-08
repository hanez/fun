/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-04
 */

/* Regex global replace opcode using POSIX regex */
#ifdef __unix__
#include <regex.h>
#include <string.h>
#include <stdlib.h>
#endif

case OP_REGEX_REPLACE: {
    Value repl = pop_value(vm);
    Value pattern = pop_value(vm);
    Value str = pop_value(vm);
    if (str.type != VAL_STRING || pattern.type != VAL_STRING || repl.type != VAL_STRING) {
        fprintf(stderr, "Runtime type error: REGEX_REPLACE expects (string, string, string)\n");
        exit(1);
    }
#ifndef __unix__
    /* Not supported: return original string */
    Value out = make_string(str.s ? str.s : "");
    free_value(repl);
    free_value(pattern);
    free_value(str);
    push_value(vm, out);
    break;
#else
    regex_t rx;
    int rc = regcomp(&rx, pattern.s ? pattern.s : "", REG_EXTENDED);
    if (rc != 0) {
        /* invalid regex -> return original */
        Value out = make_string(str.s ? str.s : "");
        free_value(repl);
        free_value(pattern);
        free_value(str);
        push_value(vm, out);
        break;
    }

    const char *s = str.s ? str.s : "";
    const char *r = repl.s ? repl.s : "";

    size_t out_cap = strlen(s) + 1;
    char *outbuf = (char*)malloc(out_cap);
    size_t out_len = 0;
    size_t pos = 0;

    enum { MAX_CAP = 16 };
    regmatch_t caps[MAX_CAP];

    while (1) {
        if (regexec(&rx, s + pos, MAX_CAP, caps, 0) != 0) {
            /* no more matches: append the rest */
            size_t rest = strlen(s + pos);
            if (out_len + rest + 1 > out_cap) {
                out_cap = out_len + rest + 1;
                outbuf = (char*)realloc(outbuf, out_cap);
            }
            memcpy(outbuf + out_len, s + pos, rest + 1);
            out_len += rest;
            break;
        }
        int mstart = (int)caps[0].rm_so;
        int mend   = (int)caps[0].rm_eo;
        if (mstart < 0 || mend < mstart) {
            /* Shouldn't happen, avoid infinite loop */
            break;
        }
        /* append prefix */
        size_t pre_len = (size_t)mstart;
        if (out_len + pre_len + 1 > out_cap) {
            out_cap = (out_len + pre_len + 1) * 2;
            outbuf = (char*)realloc(outbuf, out_cap);
        }
        memcpy(outbuf + out_len, s + pos, pre_len);
        out_len += pre_len;

        /* append replacement (no backref expansion for simplicity) */
        size_t rlen = strlen(r);
        if (out_len + rlen + 1 > out_cap) {
            out_cap = (out_len + rlen + 1) * 2;
            outbuf = (char*)realloc(outbuf, out_cap);
        }
        memcpy(outbuf + out_len, r, rlen);
        out_len += rlen;

        /* advance */
        pos += (size_t)mend;
        if (mend == 0) { /* prevent zero-length match infinite loop */
            if (pos < strlen(s)) {
                if (out_len + 1 > out_cap) { out_cap = out_len + 2; outbuf = (char*)realloc(outbuf, out_cap);}                
                outbuf[out_len++] = s[pos++];
            } else {
                break;
            }
        }
    }

    Value out = make_string(outbuf ? outbuf : "");
    if (outbuf) free(outbuf);
    regfree(&rx);
    free_value(repl);
    free_value(pattern);
    free_value(str);
    push_value(vm, out);
    break;
#endif
}
