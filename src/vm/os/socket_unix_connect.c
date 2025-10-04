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

#include <string.h>
#ifdef __unix__
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#endif

case OP_SOCK_UNIX_CONNECT: {
    /* Pops path; returns fd (>0) or 0 */
    Value pathv = pop_value(vm);
    int fd = 0;
#ifdef __unix__
    if (pathv.type != VAL_STRING) {
        fprintf(stderr, "Runtime type error: unix_connect expects (string path)\n");
        free_value(pathv);
        push_value(vm, make_int(0));
        break;
    }
    char *path = value_to_string_alloc(&pathv);
    if (path) {
        int s = socket(AF_UNIX, SOCK_STREAM, 0);
        if (s >= 0) {
            struct sockaddr_un addr;
            memset(&addr, 0, sizeof(addr));
            addr.sun_family = AF_UNIX;
            size_t maxlen = sizeof(addr.sun_path) - 1;
            strncpy(addr.sun_path, path, maxlen);
            addr.sun_path[maxlen] = '\0';
            if (connect(s, (struct sockaddr*)&addr, sizeof(addr)) == 0) {
                fd = s;
            } else {
                close(s);
            }
        }
        free(path);
    }
#endif
    free_value(pathv);
    push_value(vm, make_int(fd > 0 ? fd : 0));
    break;
}
