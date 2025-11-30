/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-11-30
 */

/* OP_INI_SET / OP_INI_UNSET / OP_INI_SAVE */
#ifdef FUN_WITH_INI
case OP_INI_SET: {
    Value vval = pop_value(vm);
    Value vkey = pop_value(vm);
    Value vsec = pop_value(vm);
    Value vh   = pop_value(vm);
    dictionary *d = ini_get((vh.type==VAL_INT)?(int)vh.i:0);
    const char *key = (vkey.type==VAL_STRING)?vkey.s:NULL;
    const char *sec = (vsec.type==VAL_STRING)?vsec.s:NULL;
    int ok = 0;
    if (d && sec && key) {
        char *valstr = value_to_string_alloc(&vval);
        if (valstr) {
            char full[1024]; snprintf(full, sizeof(full), "%s:%s", sec, key);
            /* iniparser 4.x does not expose iniparser_set; use dictionary_set */
            if (dictionary_set(d, full, valstr) == 0) ok = 1; /* 0 means success */
            free(valstr);
        }
    }
    free_value(vval); free_value(vkey); free_value(vsec); free_value(vh);
    push_value(vm, make_int(ok));
    break;
}

case OP_INI_UNSET: {
    Value vkey = pop_value(vm);
    Value vsec = pop_value(vm);
    Value vh   = pop_value(vm);
    dictionary *d = ini_get((vh.type==VAL_INT)?(int)vh.i:0);
    const char *key = (vkey.type==VAL_STRING)?vkey.s:NULL;
    const char *sec = (vsec.type==VAL_STRING)?vsec.s:NULL;
    int ok = 0;
    if (d && sec && key) {
        char full[1024]; snprintf(full, sizeof(full), "%s:%s", sec, key);
        /* iniparser 4.2.6 dictionary_unset returns void; assume success if inputs are valid */
        dictionary_unset(d, full);
        ok = 1;
    }
    free_value(vkey); free_value(vsec); free_value(vh);
    push_value(vm, make_int(ok));
    break;
}

case OP_INI_SAVE: {
    Value vpath = pop_value(vm);
    Value vh    = pop_value(vm);
    const char *path = (vpath.type==VAL_STRING)?vpath.s:NULL;
    dictionary *d = ini_get((vh.type==VAL_INT)?(int)vh.i:0);
    int ok = 0;
    if (d && path) {
        FILE *f = fopen(path, "w");
        if (f) { iniparser_dump_ini(d, f); fclose(f); ok = 1; }
    }
    free_value(vpath); free_value(vh);
    push_value(vm, make_int(ok));
    break;
}
#endif
