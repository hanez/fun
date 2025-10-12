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

case OP_SOCK_TCP_LISTEN: {
    /* Pops backlog, port; pushes listen fd (>0) or 0 */
    Value backlogv = pop_value(vm);
    Value portv = pop_value(vm);
    int fd = 0;
#ifdef __unix__
    if (portv.type != VAL_INT || backlogv.type != VAL_INT) {
        fprintf(stderr, "Runtime type error: tcp_listen expects (int port, int backlog)\n");
        free_value(backlogv);
        free_value(portv);
        push_value(vm, make_int(0));
        break;
    }
    int port = (int)portv.i;
    int backlog = (int)backlogv.i;
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s >= 0) {
        int yes = 1;
        setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons((uint16_t)port);
        if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) == 0) {
            if (listen(s, backlog > 0 ? backlog : 1) == 0) {
                fd = s;
            } else {
                close(s);
            }
        } else {
            close(s);
        }
    }
#else
    (void)fd;
#endif
    free_value(backlogv);
    free_value(portv);
    push_value(vm, make_int(fd > 0 ? fd : 0));
    break;
}
