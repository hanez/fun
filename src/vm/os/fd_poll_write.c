/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file fd_poll_write.c
 * @brief Implements OP_FD_POLL_WRITE to check if a file descriptor is writable.
 *
 * Behavior:
 * - Pops timeout_ms (int) and fd (int); waits up to timeout for writability; pushes 1 if writable, 0 otherwise.
 * - On non-UNIX platforms, returns 0 (unsupported).
 *
 * Errors:
 * - If types are wrong, prints an error and returns 0.
 */

case OP_FD_POLL_WRITE: {
  /* Pops timeout_ms:int, fd:int; pushes 1 if writable, 0 on timeout, -1 on error */
  Value to = pop_value(vm);
  Value fdv = pop_value(vm);
  int rc = -1;
#ifdef __unix__
  if (fdv.type != VAL_INT || to.type != VAL_INT) {
    fprintf(stderr, "Runtime type error: fd_poll_write expects (int fd, int timeout_ms)\n");
    rc = -1;
  } else {
    int fd = (int)fdv.i;
    int timeout_ms = (int)to.i;
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLOUT;
    pfd.revents = 0;
    int pr = poll(&pfd, 1, timeout_ms);
    if (pr > 0) {
      if (pfd.revents & (POLLERR | POLLHUP | POLLNVAL)) {
        rc = 0; /* treat as not writable */
      } else if (pfd.revents & POLLOUT) {
        rc = 1;
      } else {
        rc = 0;
      }
    } else if (pr == 0) {
      rc = 0; /* timeout */
    } else {
      rc = -1; /* error */
    }
  }
#else
  (void)to; (void)fdv;
#endif
  free_value(to);
  free_value(fdv);
  push_value(vm, make_int(rc));
  break;
}
