/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-12-10 (split from getters.c)
 */

/* OP_INI_GET_INT */
#ifdef FUN_WITH_INI
case OP_INI_GET_INT: {
    Value vdef = pop_value(vm);
    Value vkey = pop_value(vm);
    Value vsec = pop_value(vm);
    Value vh   = pop_value(vm);
    int def = (vdef.type==VAL_INT) ? (int)vdef.i : 0;
    const char *key = (vkey.type==VAL_STRING) ? vkey.s : NULL;
    const char *sec = (vsec.type==VAL_STRING) ? vsec.s : NULL;
    int h = (vh.type==VAL_INT) ? (int)vh.i : 0;
    dictionary *d = ini_get(h);
    int outi = def;
    if (d && sec && key) {
        char full[1024]; char alt[1024];
        ini_make_full_key(full, sizeof(full), sec, key);
        memcpy(alt, full, sizeof(alt));
        for (size_t i = 0; i < sizeof(alt) && alt[i]; ++i) { if (alt[i] == ':') { alt[i] = '.'; break; } }
        const char *s = iniparser_getstring(d, full, NULL);
        if (!s) s = iniparser_getstring(d, alt, NULL);
        if (s) {
            /* strip optional quotes and parse */
            char buf[256];
            size_t n = strlen(s);
            if (n >= 2 && ((s[0]=='"' && s[n-1]=='"') || (s[0]=='\'' && s[n-1]=='\''))) {
                size_t copy = (n-2) < sizeof(buf)-1 ? (n-2) : sizeof(buf)-1;
                memcpy(buf, s+1, copy); buf[copy] = '\0';
                s = buf;
            }
            /* skip leading spaces */
            while (*s && (unsigned char)*s <= ' ') s++;
            char *endp = NULL;
            long v = strtol(s, &endp, 10);
            if (endp && endp != s) outi = (int)v; else outi = def;
        } else {
            outi = def;
        }
    }
    free_value(vdef); free_value(vkey); free_value(vsec); free_value(vh);
    push_value(vm, make_int(outi));
    break;
}
#endif
