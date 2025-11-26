/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-11-26
 */

/**
 * OP_SQLITE_OPEN: (path:string) -> handle:int (>0) or 0 on error
 */
case OP_SQLITE_OPEN: {
#ifdef FUN_WITH_SQLITE
    Value vpath = pop_value(vm);
    char *path = value_to_string_alloc(&vpath);
    free_value(vpath);
    if (!path) { push_value(vm, make_int(0)); break; }
    sqlite3 *db = NULL;
    int rc = sqlite3_open(path, &db);
    free(path);
    if (rc != SQLITE_OK || !db) {
        if (db) sqlite3_close(db);
        push_value(vm, make_int(0));
        break;
    }
    SqlHandle *h = sql_reg_add(db);
    if (!h) { sqlite3_close(db); push_value(vm, make_int(0)); break; }
    push_value(vm, make_int(h->id));
#else
    Value v = pop_value(vm); free_value(v);
    push_value(vm, make_int(0));
#endif
    break;
}
