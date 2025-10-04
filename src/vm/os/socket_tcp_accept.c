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
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

case OP_SOCK_TCP_ACCEPT: {
    /* Pops listen fd; pushes client fd (>0) or 0 */
    Value fdv = pop_value(vm);
    int client = 0;
#ifdef __unix__
    if (fdv.type != VAL_INT) {
        fprintf(stderr, "Runtime type error: tcp_accept expects (int listen_fd)\n");
        free_value(fdv);
        push_value(vm, make_int(0));
        break;
    }
    int s = (int)fdv.i;
    int c = accept(s, NULL, NULL);
    if (c >= 0) client = c;
#endif
    free_value(fdv);
    push_value(vm, make_int(client > 0 ? client : 0));
    break;
}
