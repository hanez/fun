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

/* OP_INI_GET_BOOL */
#ifdef FUN_WITH_INI
case OP_INI_GET_BOOL: {
    Value vdef = pop_value(vm);
    Value vkey = pop_value(vm);
    Value vsec = pop_value(vm);
    Value vh   = pop_value(vm);
    int def = (vdef.type==VAL_INT||vdef.type==VAL_BOOL) ? (int)vdef.i : 0;
    const char *key = (vkey.type==VAL_STRING) ? vkey.s : NULL;
    const char *sec = (vsec.type==VAL_STRING) ? vsec.s : NULL;
    int h = (vh.type==VAL_INT) ? (int)vh.i : 0;
    dictionary *d = ini_get(h);
    int outb = def;
    if (d && sec && key) {
        char full[1024]; char alt[1024];
        ini_make_full_key(full, sizeof(full), sec, key);
        memcpy(alt, full, sizeof(alt));
        for (size_t i = 0; i < sizeof(alt) && alt[i]; ++i) { if (alt[i] == ':') { alt[i] = '.'; break; } }
        const char *s = iniparser_getstring(d, full, NULL);
        if (!s) s = iniparser_getstring(d, alt, NULL);
        if (s) {
            /* normalize and parse boolean */
            char buf[256];
            size_t n = strlen(s);
            if (n >= 2 && ((s[0]=='"' && s[n-1]=='"') || (s[0]=='\'' && s[n-1]=='\''))) {
                size_t copy = (n-2) < sizeof(buf)-1 ? (n-2) : sizeof(buf)-1;
                memcpy(buf, s+1, copy); buf[copy] = '\0';
                s = buf;
            }
            /* trim spaces */
            while (*s && (unsigned char)*s <= ' ') s++;
            /* lower copy for textual booleans */
            char lb[256]; size_t li=0; for (; s[li] && li < sizeof(lb)-1; ++li) lb[li] = (char)tolower((unsigned char)s[li]); lb[li]='\0';
            if (strcmp(lb, "true")==0 || strcmp(lb, "yes")==0 || strcmp(lb, "on")==0) {
                outb = 1;
            } else if (strcmp(lb, "false")==0 || strcmp(lb, "no")==0 || strcmp(lb, "off")==0) {
                outb = 0;
            } else {
                /* numeric */
                char *endp=NULL; long v = strtol(lb, &endp, 10);
                outb = (endp && endp!=lb) ? (v!=0) : def;
            }
        } else {
            outb = def;
        }
    }
    free_value(vdef); free_value(vkey); free_value(vsec); free_value(vh);
    push_value(vm, make_int(outb ? 1 : 0));
    break;
}
#endif
