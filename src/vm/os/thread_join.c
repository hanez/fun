/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file thread_join.c
 * @brief Implements OP_THREAD_JOIN to wait for a spawned thread and get its result.
 *
 * Behavior:
 * - Pops thread id (int); waits for the thread to finish; pushes the result Value produced by the thread.
 * - If the thread failed or id is invalid, pushes Nil.
 *
 * Errors:
 * - If argument type is wrong, prints an error and pushes Nil.
 */

case OP_THREAD_JOIN: {
  Value vtid = pop_value(vm);
  if (vtid.type != VAL_INT) {
    fprintf(stderr, "Runtime type error: thread_join expects thread id (int)\n");
    push_value(vm, make_nil());
    free_value(vtid);
    break;
  }
  Value res = fun_thread_join((int)vtid.i);
  free_value(vtid);
  push_value(vm, res); /* takes ownership */
  break;
}
