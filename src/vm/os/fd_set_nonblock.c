/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file fd_set_nonblock.c
 * @brief Implements OP_FD_SET_NONBLOCK to toggle O_NONBLOCK on a file descriptor.
 *
 * Behavior:
 * - Pops on (int, 0/1) and fd (int); sets or clears O_NONBLOCK via fcntl; pushes 1 on success, 0 otherwise.
 * - On non-UNIX platforms, returns 0 (unsupported).
 *
 * Errors:
 * - If types are wrong, prints an error and returns 0.
 */

case OP_FD_SET_NONBLOCK: {
  /* Pops on:int (0/1), fd:int; pushes 1 on success, 0 on error/unsupported */
  Value onv = pop_value(vm);
  Value fdv = pop_value(vm);
  int ok = 0;
#ifdef __unix__
  if (fdv.type != VAL_INT || onv.type != VAL_INT) {
    fprintf(stderr, "Runtime type error: fd_set_nonblock expects (int fd, int on)\n");
    ok = 0;
  } else {
    int fd = (int)fdv.i;
    int on = (int)onv.i;
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags >= 0) {
      if (on)
        flags |= O_NONBLOCK;
      else
        flags &= ~O_NONBLOCK;
      if (fcntl(fd, F_SETFL, flags) == 0) ok = 1;
    }
  }
#else
  (void)onv; (void)fdv;
#endif
  free_value(onv);
  free_value(fdv);
  push_value(vm, make_int(ok ? 1 : 0));
  break;
}
