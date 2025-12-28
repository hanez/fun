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

case OP_SERIAL_CLOSE: {
    /* Pops fd (int); returns 1/0 */
    Value fdv = pop_value(vm);
    int ok = 0;
#ifdef __unix__
    if (fdv.type != VAL_INT) {
        fprintf(stderr, "Runtime type error: serial_close expects (int fd)\n");
    } else {
        int fd = (int)fdv.i;
        if (close(fd) == 0) ok = 1;
    }
#endif
    free_value(fdv);
    push_value(vm, make_int(ok));
    break;
}
