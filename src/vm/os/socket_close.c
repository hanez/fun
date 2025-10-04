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

#ifdef __unix__
#include <unistd.h>
#endif

case OP_SOCK_CLOSE: {
    /* Pops fd; returns 1/0 */
    Value fdv = pop_value(vm);
    int ok = 0;
#ifdef __unix__
    if (fdv.type != VAL_INT) {
        fprintf(stderr, "Runtime type error: sock_close expects (int fd)\n");
        free_value(fdv);
        push_value(vm, make_int(0));
        break;
    }
    int fd = (int)fdv.i;
    ok = (close(fd) == 0) ? 1 : 0;
#endif
    free_value(fdv);
    push_value(vm, make_int(ok));
    break;
}
