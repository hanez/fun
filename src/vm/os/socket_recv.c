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

#include <stdlib.h>

#ifdef __unix__
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

case OP_SOCK_RECV: {
    /* Pops maxlen, fd; pushes data string ("" on EOF/error) */
    Value maxv = pop_value(vm);
    Value fdv = pop_value(vm);
    char *out = NULL;
#ifdef __unix__
    if (fdv.type != VAL_INT || maxv.type != VAL_INT) {
        fprintf(stderr, "Runtime type error: sock_recv expects (int fd, int maxlen)\n");
        free_value(maxv);
        free_value(fdv);
        push_value(vm, make_string(""));
        break;
    }
    int fd = (int)fdv.i;
    int maxlen = (int)maxv.i;
    if (maxlen <= 0) maxlen = 4096;
    if (maxlen > 1<<20) maxlen = 1<<20; /* cap at 1MB */
    out = (char*)malloc((size_t)maxlen + 1);
    if (out) {
        ssize_t n = recv(fd, out, (size_t)maxlen, 0);
        if (n <= 0) {
            free(out); out = NULL;
        } else {
            out[n] = '\0';
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
