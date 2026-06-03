/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file cmd.c
 * @brief Implements the OP_REDIS_CMD opcode (conditional build).
 *
 * Executes a Redis command using the synchronous hiredis API on a previously
 * opened handle and converts the reply to a Fun Value.
 *
 * Stack effect
 * ------------
 * OP_REDIS_CMD: (handle:int, cmd:string) -> reply:Value
 *
 * Behavior
 * --------
 * - The command string uses Redis inline protocol formatting (e.g.,
 *   "PING", "SET key val", "LRANGE list 0 -1").
 * - When FUN_WITH_REDIS is enabled, the opcode looks up the connection by
 *   handle id, issues redisCommand(), and converts the resulting reply to a
 *   Fun Value:
 *     - status/string -> string
 *     - integer       -> int
 *     - nil           -> nil
 *     - array         -> array of converted elements
 *   On errors or lookup failures, pushes Nil.
 * - When FUN_WITH_REDIS is disabled, pops arguments and pushes Nil.
 */

case OP_REDIS_CMD: {
#ifdef FUN_WITH_REDIS
  Value vcmd = pop_value(vm);
  Value vh = pop_value(vm);
  int hid = (int)vh.i;
  char *cmd = value_to_string_alloc(&vcmd);
  free_value(vh);
  free_value(vcmd);
  RedisHandle *h = redis_reg_get(hid);
  if (!h || !h->ctx || !cmd) { if (cmd) free(cmd); push_value(vm, make_nil()); break; }
  redisReply *r = (redisReply *)redisCommand(h->ctx, cmd);
  free(cmd);
  if (!r) { push_value(vm, make_nil()); break; }
  Value out = hiredis_reply_to_value(r);
  freeReplyObject(r);
  push_value(vm, out);
#else
  Value v1 = pop_value(vm); free_value(v1);
  Value v2 = pop_value(vm); free_value(v2);
  push_value(vm, make_nil());
#endif
  break;
}
