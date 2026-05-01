/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file socket_unix_listen.c
 * @brief Implements OP_SOCK_UNIX_LISTEN to create a UNIX domain listening socket.
 *
 * Behavior:
 * - Pops path (string); creates/binds/listens on AF_UNIX socket; pushes fd (>0) or 0 on failure.
 * - The path may be unlinked before bind to avoid EADDRINUSE.
 *
 * Errors:
 * - On wrong type or OS errors, prints an error and pushes 0. Non-UNIX platforms return 0.
 */

case OP_SOCK_UNIX_LISTEN: {
  /* Pops backlog, path; returns listen fd (>0) or 0 */
  Value backlogv = pop_value(vm);
  Value pathv = pop_value(vm);
  int fd = 0;
#ifdef __unix__
  if (pathv.type != VAL_STRING || backlogv.type != VAL_INT) {
    fprintf(stderr, "Runtime type error: unix_listen expects (string path, int backlog)\n");
    free_value(backlogv);
    free_value(pathv);
    push_value(vm, make_int(0));
    break;
  }
  char *path = value_to_string_alloc(&pathv);
  int backlog = (int)backlogv.i;
  if (path) {
    int s = socket(AF_UNIX, SOCK_STREAM, 0);
    if (s >= 0) {
      struct sockaddr_un addr;
      memset(&addr, 0, sizeof(addr));
      addr.sun_family = AF_UNIX;
      size_t maxlen = sizeof(addr.sun_path) - 1;
      strncpy(addr.sun_path, path, maxlen);
      addr.sun_path[maxlen] = '\0';
      unlink(addr.sun_path); /* best effort */
      if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) == 0) {
        if (listen(s, backlog > 0 ? backlog : 1) == 0) {
          fd = s;
        } else {
          close(s);
        }
      } else {
        close(s);
      }
    }
    free(path);
  }
#endif
  free_value(backlogv);
  free_value(pathv);
  push_value(vm, make_int(fd > 0 ? fd : 0));
  break;
}
