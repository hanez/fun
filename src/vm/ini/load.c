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

/* OP_INI_LOAD: pops path string; pushes handle (>0) or 0 */
#ifdef FUN_WITH_INI
case OP_INI_LOAD: {
    Value vpath = pop_value(vm);
    const char *path = (vpath.type == VAL_STRING && vpath.s) ? vpath.s : NULL;
    int h = 0;
    if (path) {
        dictionary *d = iniparser_load(path);
        if (d) {
            h = ini_alloc_handle(d);
            if (!h) { iniparser_freedict(d); }
        }
    }
    free_value(vpath);
    push_value(vm, make_int(h));
    break;
}
#endif
