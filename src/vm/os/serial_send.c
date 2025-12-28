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

case OP_SERIAL_SEND: {
    /* Pops data (string), fd (int); returns bytes sent (int) */
    Value datav = pop_value(vm);
    Value fdv = pop_value(vm);
    int sent = -1;
#ifdef __unix__
    if (fdv.type != VAL_INT || datav.type != VAL_STRING) {
        fprintf(stderr, "Runtime type error: serial_send expects (int fd, string data)\n");
    } else {
        int fd = (int)fdv.i;
        const char *buf = datav.s ? datav.s : "";
        size_t len = strlen(buf);
        ssize_t n = write(fd, buf, len);
        if (n >= 0) sent = (int)n;
    }
#endif
    free_value(datav);
    free_value(fdv);
    push_value(vm, make_int(sent));
    break;
}
