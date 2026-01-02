/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-12-10 (split from set_unset_save.c)
 */

/* OP_INI_SET */
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
            char full[1024]; char alt[1024];
            ini_make_full_key(full, sizeof(full), sec, key);
            memcpy(alt, full, sizeof(alt));
            for (size_t i = 0; i < sizeof(alt) && alt[i]; ++i) { if (alt[i] == ':') { alt[i] = '.'; break; } }
            /* iniparser 4.x does not expose iniparser_set; use dictionary_set */
            if (dictionary_set(d, full, valstr) == 0) {
                ok = 1; /* 0 means success */
            } else if (dictionary_set(d, alt, valstr) == 0) {
                ok = 1;
            }
            free(valstr);
        }
    }
    free_value(vval); free_value(vkey); free_value(vsec); free_value(vh);
    push_value(vm, make_int(ok));
    break;
}
#endif
