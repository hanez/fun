/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-02
 */

/* PCSC release */
case OP_PCSC_RELEASE: {
#ifdef FUN_WITH_PCSC
    Value vid = pop_value(vm);
    int id = (int)vid.i;
    free_value(vid);
    pcsc_ctx_entry *e = pcsc_get_ctx(id);
    if (!e) { push_value(vm, make_int(0)); break; }
    SCardReleaseContext(e->ctx);
    e->in_use = 0;
    e->ctx = 0;
    push_value(vm, make_int(1));
#else
    Value vid = pop_value(vm); free_value(vid);
    push_value(vm, make_int(0));
#endif
    break;
}
