/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file socket_tcp_accept.c
 * @brief Implements OP_SOCK_TCP_ACCEPT to accept an incoming TCP connection.
 *
 * Behavior:
 * - Pops listening socket fd (int); accepts one connection; pushes new client fd (>0) or 0.
 *
 * Errors:
 * - On wrong type or OS errors, prints an error and pushes 0. Non-UNIX platforms return 0.
 */

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
