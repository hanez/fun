/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file parse.c
 * @brief VM opcode snippet for parsing the incoming HTTP request via kcgi.
 *
 * This snippet is included by vm.c and implements the OP_KCGI_PARSE
 * instruction. It invokes kcgi to parse the current HTTP request. If a
 * previous request object exists in g_kcgi_req, it is freed first to avoid
 * leaks. On success, the global g_kcgi_req is updated and the parsed request
 * is converted to a Fun Value via kreq_to_fun() and pushed onto the stack.
 * On failure, Nil is pushed.
 *
 * Build gating: compiled only when FUN_WITH_KCGI is enabled. When disabled,
 * the opcode simply pushes Nil.
 *
 * Stack effect (with FUN_WITH_KCGI):
 * - Pops: (none)
 * - Pushes: Fun Value representing the request on success, or Nil on error
 */

/* KCGI_PARSE */
case OP_KCGI_PARSE: {
#ifdef FUN_WITH_KCGI
  if (g_kcgi_req) { /* safety: free previous if any */
    kcgi_free_request(g_kcgi_req);
    g_kcgi_req = NULL;
  }
  struct kreq *r = NULL;
  if (!kcgi_parse_request(&r)) {
    push_value(vm, make_nil());
    break;
  }
  g_kcgi_req = r;
  Value v = kreq_to_fun(r);
  push_value(vm, v);
#else
  push_value(vm, make_nil());
#endif
  break;
}
