/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file close.c
 * @brief Implements the OP_REDIS_CLOSE opcode (conditional build).
 *
 * Closes a previously opened Redis connection and removes its registry entry.
 *
 * Stack effect
 * ------------
 * OP_REDIS_CLOSE: (handle:int) -> Nil
 *
 * Behavior
 * --------
 * - When FUN_WITH_REDIS is enabled, looks up the handle, calls redisFree() on
 *   the underlying connection if present, clears the pointer, and removes the
 *   registry entry. Always pushes Nil.
 * - When FUN_WITH_REDIS is disabled, pops the argument and pushes Nil.
 */

case OP_REDIS_CLOSE: {
#ifdef FUN_WITH_REDIS
  Value vh = pop_value(vm);
  int hid = (int)vh.i;
  free_value(vh);
  RedisHandle *h = redis_reg_get(hid);
  if (h && h->ctx) {
    redisFree(h->ctx);
    h->ctx = NULL;
  }
  redis_reg_del(hid);
  push_value(vm, make_nil());
#else
  Value v1 = pop_value(vm); free_value(v1);
  push_value(vm, make_nil());
#endif
  break;
}
