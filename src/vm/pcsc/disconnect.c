/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file disconnect.c
 * @brief Implements the OP_PCSC_DISCONNECT opcode (conditional build).
 *
 * Disconnects a previously connected smart-card handle and frees the
 * corresponding registry slot. When PCSC support is disabled at build time,
 * this opcode returns 0 after consuming its argument.
 */

/**
 * OP_PCSC_DISCONNECT: (handle_id:int) -> int
 *
 * - Pops: handle_id.
 * - Pushes: 1 on success; 0 on error (invalid id/not found) or when disabled.
 * - Notes: Uses SCardDisconnect(..., SCARD_LEAVE_CARD).
 */

/* PCSC disconnect */
case OP_PCSC_DISCONNECT: {
#ifdef FUN_WITH_PCSC
  Value vh = pop_value(vm);
  int hid = (int)vh.i;
  free_value(vh);
  pcsc_card_entry *ce = pcsc_get_card(hid);
  if (!ce) {
    push_value(vm, make_int(0));
    break;
  }
  SCardDisconnect(ce->h, SCARD_LEAVE_CARD);
  ce->in_use = 0;
  ce->h = 0;
  ce->proto = 0;
  push_value(vm, make_int(1));
#else
  Value vh = pop_value(vm);
  free_value(vh);
  push_value(vm, make_int(0));
#endif
  break;
}
