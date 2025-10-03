/**
* This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-04
 */

/**
 * @file date_format.c
 * @brief Implements OP_DATE_FORMAT to format an epoch milliseconds timestamp using strftime.
 *
 * Behavior:
 * - Pops fmt string, then milliseconds since Unix epoch (int).
 * - Formats the local time according to fmt and pushes a string.
 *
 * Errors:
 * - If types are wrong, prints error and pushes empty string to keep stack safe.
 */

#include <time.h>
#include <string.h>
#include <stdlib.h>

case OP_DATE_FORMAT: {
    Value fmt = pop_value(vm);
    Value ms  = pop_value(vm);
    if (ms.type != VAL_INT || fmt.type != VAL_STRING) {
        fprintf(stderr, "DATE_FORMAT expects (fmt:string, ms:int)\n");
        if (ms.type != VAL_NIL) free_value(ms);
        if (fmt.type != VAL_NIL) free_value(fmt);
        push_value(vm, make_string(""));
        break;
    }
    time_t secs = (time_t)(ms.i / 1000);
    struct tm tmv;
#if defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 1
    localtime_r(&secs, &tmv);
#else
    struct tm *ptm = localtime(&secs);
    if (!ptm) {
        free_value(ms); free_value(fmt);
        push_value(vm, make_string(""));
        break;
    }
    tmv = *ptm;
#endif
    /* Determine buffer size by attempting once with a reasonable size and retrying if needed */
    size_t cap = 128;
    char *buf = (char*)malloc(cap);
    size_t n = strftime(buf, cap, fmt.s, &tmv);
    if (n == 0) {
        /* try larger */
        cap = 512;
        buf = (char*)realloc(buf, cap);
        n = strftime(buf, cap, fmt.s, &tmv);
    }
    Value out;
    if (n == 0) {
        out = make_string("");
    } else {
        out = make_string(buf);
    }
    free(buf);
    free_value(ms);
    free_value(fmt);
    push_value(vm, out);
    break;
}
