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
 * @brief Implements the OP_PCSC_CONNECT opcode (conditional build).
 *
 * Connects to a smart card in the specified reader using an existing PC/SC
 * context. On success, allocates/returns a card handle id from the internal
 * registry. When PCSC support is disabled at build time, this opcode returns 0.
 */

/**
 * OP_PCSC_CONNECT: (ctx_id:int, reader_name:any) -> int
 *
 * - Pops: reader_name (converted to string), then ctx_id.
 * - Pushes: card handle id (>0) on success; 0 on error/invalid inputs or when
 *   FUN_WITH_PCSC is disabled.
 * - Notes: Uses SCARD_SHARE_SHARED and negotiates T0/T1 protocols.
 */

/* PCSC connect */
case OP_PCSC_CONNECT: {
#ifdef FUN_WITH_PCSC
  /* Stack: [..., ctx_id, reader_name] -> pops reader_name first, then ctx_id */
  Value vreader = pop_value(vm);
  Value vctx = pop_value(vm);

  int ctx_id = (int)vctx.i;
  free_value(vctx);
  char *rname = value_to_string_alloc(&vreader);
  free_value(vreader);

  pcsc_ctx_entry *e = pcsc_get_ctx(ctx_id);
  if (!e || !rname) {
    if (rname) free(rname);
    push_value(vm, make_int(0));
    break;
  }

  int hslot = pcsc_alloc_card_slot();
  if (!hslot) {
    free(rname);
    push_value(vm, make_int(0));
    break;
  }
  pcsc_card_entry *ce = pcsc_get_card(hslot);
  DWORD dwActive = 0;
  LONG rv = SCardConnect(e->ctx, rname, SCARD_SHARE_SHARED,
                         SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1,
                         &ce->h, &dwActive);
  free(rname);
  if (rv != SCARD_S_SUCCESS) {
    ce->in_use = 0;
    push_value(vm, make_int(0));
    break;
  }
  ce->proto = dwActive;
  push_value(vm, make_int(hslot));
#else
  Value vreader = pop_value(vm);
  free_value(vreader);
  Value vctx = pop_value(vm);
  free_value(vctx);
  push_value(vm, make_int(0));
#endif
  break;
}
