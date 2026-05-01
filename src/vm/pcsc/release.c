/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file release.c
 * @brief Implements the OP_PCSC_RELEASE opcode (conditional build).
 *
 * Releases a previously established PC/SC context and frees its registry slot.
 * When PCSC support is disabled at build time, this opcode returns 0 after
 * consuming its argument.
 */

/**
 * OP_PCSC_RELEASE: (ctx_id:int) -> int
 *
 * - Pops: ctx_id.
 * - Pushes: 1 on success; 0 on error (invalid id/not found) or when disabled.
 */

/* PCSC release */
case OP_PCSC_RELEASE: {
#ifdef FUN_WITH_PCSC
  Value vid = pop_value(vm);
  int id = (int)vid.i;
  free_value(vid);
  pcsc_ctx_entry *e = pcsc_get_ctx(id);
  if (!e) {
    push_value(vm, make_int(0));
    break;
  }
  SCardReleaseContext(e->ctx);
  e->in_use = 0;
  e->ctx = 0;
  push_value(vm, make_int(1));
#else
  Value vid = pop_value(vm);
  free_value(vid);
  push_value(vm, make_int(0));
#endif
  break;
}
