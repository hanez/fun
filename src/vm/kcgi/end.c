/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file end.c
 * @brief VM opcode snippet to finalize kcgi handling and release state.
 *
 * This snippet is included by vm.c and implements the OP_KCGI_END
 * instruction. When FUN_WITH_KCGI is enabled, it releases any currently
 * stored request (g_kcgi_req) and pushes 1, indicating success. When kcgi
 * support is disabled, it simply pushes 0.
 *
 * Stack effect:
 * - Pops: (none)
 * - Pushes: int 1 when FUN_WITH_KCGI, otherwise int 0
 */

/* KCGI_END */
case OP_KCGI_END: {
#ifdef FUN_WITH_KCGI
  if (g_kcgi_req) { kcgi_free_request(g_kcgi_req); g_kcgi_req = NULL; }
  push_value(vm, make_int(1));
#else
  push_value(vm, make_int(0));
#endif
  break;
}
