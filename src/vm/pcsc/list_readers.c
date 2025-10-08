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

/* PCSC list_readers */
case OP_PCSC_LIST_READERS: {
#ifdef FUN_WITH_PCSC
    /* Pop context id; return [] if anything goes wrong */
    Value vid = pop_value(vm);
    int id = (int)vid.i;
    free_value(vid);

    pcsc_ctx_entry *e = pcsc_get_ctx(id);
    if (!e) { push_value(vm, make_array_from_values(NULL, 0)); break; }

    DWORD sz = 0;
    LONG rv = SCardListReaders(e->ctx, NULL, NULL, &sz);
    if (rv != SCARD_S_SUCCESS || sz == 0) {
        push_value(vm, make_array_from_values(NULL, 0));
        break;
    }

    char *msz = (char*)malloc(sz);
    if (!msz) { push_value(vm, make_array_from_values(NULL, 0)); break; }

    rv = SCardListReaders(e->ctx, NULL, msz, &sz);
    if (rv != SCARD_S_SUCCESS) {
        free(msz);
        push_value(vm, make_array_from_values(NULL, 0));
        break;
    }

    Value vals[64];
    int count = 0;
    char *p = msz;
    while (*p && count < (int)(sizeof(vals)/sizeof(vals[0]))) {
        size_t n = strlen(p);
        vals[count++] = make_string(p);
        p += n + 1;
    }
    Value arr = make_array_from_values(vals, count);
    for (int i = 0; i < count; ++i) free_value(vals[i]);
    free(msz);

    if (arr.type != VAL_ARRAY) {
        push_value(vm, make_array_from_values(NULL, 0));
    } else {
        push_value(vm, arr);
    }
#else
    /* Fallback (no PCSC): consume ctx id and return [] to keep the stack consistent */
    Value vid = pop_value(vm);
    free_value(vid);
    push_value(vm, make_array_from_values(NULL, 0));
#endif
    break;
}
