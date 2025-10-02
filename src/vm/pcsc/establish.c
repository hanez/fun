/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-02
 */

/* PCSC establish */
case OP_PCSC_ESTABLISH: {
#ifdef FUN_WITH_PCSC
    int slot = pcsc_alloc_ctx_slot();
    if (!slot) { push_value(vm, make_int(0)); break; }
    pcsc_ctx_entry *e = pcsc_get_ctx(slot);
    if (!e) { push_value(vm, make_int(0)); break; }
    LONG rv = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &e->ctx);
    if (rv != SCARD_S_SUCCESS) { e->in_use = 0; push_value(vm, make_int(0)); break; }
    push_value(vm, make_int(slot));
#else
    push_value(vm, make_int(0));
#endif
    break;
}
