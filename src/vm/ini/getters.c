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

/* OP_INI_GET_* implementations */
#ifdef FUN_WITH_INI
case OP_INI_GET_STRING: {
    Value vdef = pop_value(vm);
    Value vkey = pop_value(vm);
    Value vsec = pop_value(vm);
    Value vh   = pop_value(vm);
    const char *def = (vdef.type==VAL_STRING && vdef.s) ? vdef.s : "";
    const char *key = (vkey.type==VAL_STRING) ? vkey.s : NULL;
    const char *sec = (vsec.type==VAL_STRING) ? vsec.s : NULL;
    int h = (vh.type==VAL_INT) ? (int)vh.i : 0;
    dictionary *d = ini_get(h);
    const char *res = def;
    if (d && sec && key) {
        char full[1024]; ini_make_full_key(full, sizeof(full), sec, key);
        const char *s = iniparser_getstring(d, full, def);
        res = s ? s : "";
    }
    free_value(vdef); free_value(vkey); free_value(vsec); free_value(vh);
    push_value(vm, make_string(res));
    break;
}

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
        char full[1024]; ini_make_full_key(full, sizeof(full), sec, key);
        outi = iniparser_getint(d, full, def);
    }
    free_value(vdef); free_value(vkey); free_value(vsec); free_value(vh);
    push_value(vm, make_int(outi));
    break;
}

case OP_INI_GET_DOUBLE: {
    Value vdef = pop_value(vm);
    Value vkey = pop_value(vm);
    Value vsec = pop_value(vm);
    Value vh   = pop_value(vm);
    double def = (vdef.type==VAL_FLOAT) ? vdef.d : (vdef.type==VAL_INT ? (double)vdef.i : 0.0);
    const char *key = (vkey.type==VAL_STRING) ? vkey.s : NULL;
    const char *sec = (vsec.type==VAL_STRING) ? vsec.s : NULL;
    int h = (vh.type==VAL_INT) ? (int)vh.i : 0;
    dictionary *d = ini_get(h);
    double outd = def;
    if (d && sec && key) {
        char full[1024]; ini_make_full_key(full, sizeof(full), sec, key);
        outd = iniparser_getdouble(d, full, def);
    }
    free_value(vdef); free_value(vkey); free_value(vsec); free_value(vh);
    push_value(vm, make_float(outd));
    break;
}

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
        char full[1024]; ini_make_full_key(full, sizeof(full), sec, key);
        outb = iniparser_getboolean(d, full, def);
    }
    free_value(vdef); free_value(vkey); free_value(vsec); free_value(vh);
    push_value(vm, make_int(outb ? 1 : 0));
    break;
}
#endif
