/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file establish.c
 * @brief Implements the OP_PCSC_ESTABLISH opcode (conditional build).
 *
 * Creates a new PC/SC context using SCardEstablishContext(SCARD_SCOPE_SYSTEM)
 * and registers it in the internal PCSC context registry when FUN_WITH_PCSC
 * is enabled. Returns the allocated context id on success, or 0 on failure.
 * When PCSC support is disabled at build time, this opcode returns 0.
 */

/**
 * OP_PCSC_ESTABLISH: () -> int
 *
 * - Returns: context id (>0) on success; 0 on error or when disabled.
 */

/* PCSC establish */
case OP_PCSC_ESTABLISH: {
#ifdef FUN_WITH_PCSC
  int slot = pcsc_alloc_ctx_slot();
  if (!slot) {
    push_value(vm, make_int(0));
    break;
  }
  pcsc_ctx_entry *e = pcsc_get_ctx(slot);
  if (!e) {
    push_value(vm, make_int(0));
    break;
  }
  LONG rv = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &e->ctx);
  if (rv != SCARD_S_SUCCESS) {
    e->in_use = 0;
    push_value(vm, make_int(0));
    break;
  }
  push_value(vm, make_int(slot));
#else
  push_value(vm, make_int(0));
#endif
  break;
}
