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

case OP_SOCK_SEND: {
    /* Pops data string, fd; pushes bytes sent (>=0) or -1 */
    Value datav = pop_value(vm);
    Value fdv = pop_value(vm);
    int sent = -1;
#ifdef __unix__
    if (fdv.type != VAL_INT || datav.type != VAL_STRING) {
        fprintf(stderr, "Runtime type error: sock_send expects (int fd, string data)\n");
        free_value(datav);
        free_value(fdv);
        push_value(vm, make_int(-1));
        break;
    }
    int fd = (int)fdv.i;
    const char *buf = datav.s ? datav.s : "";
    size_t len = strlen(buf);
    ssize_t n = send(fd, buf, len, 0);
    if (n >= 0) sent = (int)n; else sent = -1;
#endif
    free_value(datav);
    free_value(fdv);
    push_value(vm, make_int(sent));
    break;
}
