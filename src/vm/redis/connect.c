/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file connect.c
 * @brief Implements the OP_REDIS_CONNECT opcode (conditional build).
 *
 * Establishes a synchronous TCP connection to a Redis-compatible server using
 * hiredis and registers the connection handle in the internal registry.
 *
 * Stack effect
 * ------------
 * OP_REDIS_CONNECT: (host:string, port:int) -> handle:int (>0) or 0 on error
 *
 * Behavior
 * --------
 * - When FUN_WITH_REDIS is enabled, attempts a blocking connect with a
 *   2-second timeout via redisConnectWithTimeout(). On success, the created
 *   redisContext* is stored in the registry and the assigned positive handle
 *   id is pushed. On failure, 0 is pushed.
 * - When FUN_WITH_REDIS is disabled, arguments are popped and 0 is pushed.
 *
 * Notes
 * -----
 * - The returned handle must be closed with OP_REDIS_CLOSE to release
 *   resources and delete the registry entry.
 */

case OP_REDIS_CONNECT: {
#ifdef FUN_WITH_REDIS
  Value vport = pop_value(vm);
  Value vhost = pop_value(vm);
  int port = (int)vport.i;
  char *host = value_to_string_alloc(&vhost);
  free_value(vport);
  free_value(vhost);
  if (!host) { push_value(vm, make_int(0)); break; }
  struct timeval tv; tv.tv_sec = 2; tv.tv_usec = 0;
  redisContext *ctx = redisConnectWithTimeout(host, port, tv);
  free(host);
  if (!ctx || ctx->err) {
    if (ctx) redisFree(ctx);
    push_value(vm, make_int(0));
    break;
  }
  RedisHandle *h = redis_reg_add(ctx);
  if (!h) {
    redisFree(ctx);
    push_value(vm, make_int(0));
    break;
  }
  push_value(vm, make_int(h->id));
#else
  Value v1 = pop_value(vm); free_value(v1);
  Value v2 = pop_value(vm); free_value(v2);
  push_value(vm, make_int(0));
#endif
  break;
}
