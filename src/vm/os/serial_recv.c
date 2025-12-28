/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-12-28
 */

#ifdef __unix__
#include <unistd.h>
#endif

case OP_SERIAL_RECV: {
    /* Pops maxlen (int), fd (int); returns data (string) */
    Value maxv = pop_value(vm);
    Value fdv = pop_value(vm);
    char *out = NULL;
#ifdef __unix__
    if (fdv.type != VAL_INT || maxv.type != VAL_INT) {
        fprintf(stderr, "Runtime type error: serial_recv expects (int fd, int maxlen)\n");
    } else {
        int fd = (int)fdv.i;
        int maxlen = (int)maxv.i;
        if (maxlen <= 0) maxlen = 4096;
        if (maxlen > 1<<20) maxlen = 1<<20; /* cap at 1MB */
        out = (char*)malloc((size_t)maxlen + 1);
        if (out) {
            ssize_t n = read(fd, out, (size_t)maxlen);
            if (n <= 0) {
                free(out);
                out = NULL;
            } else {
                out[n] = '\0';
            }
        }
    }
#endif
    free_value(maxv);
    free_value(fdv);
    Value s = make_string(out ? out : "");
    if (out) free(out);
    push_value(vm, s);
    break;
}
