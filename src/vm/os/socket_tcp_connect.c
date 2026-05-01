/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file socket_tcp_connect.c
 * @brief Implements OP_SOCK_TCP_CONNECT to open a TCP connection.
 *
 * Behavior:
 * - Pops port (int) and host (string); attempts to connect and pushes fd (>0) on success or 0 on failure.
 * - Name resolution is performed via getaddrinfo; first successful connect wins.
 *
 * Errors:
 * - If argument types are wrong, prints an error and pushes 0.
 * - On non-UNIX platforms, returns 0 (unsupported).
 */

case OP_SOCK_TCP_CONNECT: {
  /* Pops port, host; pushes fd (>0) or 0 */
  Value portv = pop_value(vm);
  Value hostv = pop_value(vm);
  int fd = 0;
#ifdef __unix__
  if (hostv.type != VAL_STRING || portv.type != VAL_INT) {
    fprintf(stderr, "Runtime type error: tcp_connect expects (string host, int port)\n");
    free_value(portv);
    free_value(hostv);
    push_value(vm, make_int(0));
    break;
  }
  char *host = value_to_string_alloc(&hostv);
  int port = (int)portv.i;
  if (host) {
    char portstr[16];
    snprintf(portstr, sizeof(portstr), "%d", port);
    struct addrinfo hints, *res = NULL, *rp;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(host, portstr, &hints, &res) == 0) {
      for (rp = res; rp != NULL; rp = rp->ai_next) {
        int s = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (s < 0) continue;
        if (connect(s, rp->ai_addr, rp->ai_addrlen) == 0) {
          fd = s;
          break;
        }
        close(s);
      }
      if (res) freeaddrinfo(res);
    }
    free(host);
  }
#endif
  free_value(portv);
  free_value(hostv);
  push_value(vm, make_int(fd > 0 ? fd : 0));
  break;
}
