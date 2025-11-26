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
 * OP_SQLITE_CLOSE: (handle:int) -> Nil
 */
case OP_SQLITE_CLOSE: {
#ifdef FUN_WITH_SQLITE
    Value vh = pop_value(vm);
    int hid = (int)vh.i;
    free_value(vh);
    SqlHandle *h = sql_reg_get(hid);
    if (h && h->db) {
        sqlite3_close(h->db);
        h->db = NULL;
        sql_reg_del(hid);
    }
    push_value(vm, make_nil());
#else
    Value v = pop_value(vm); free_value(v);
    push_value(vm, make_nil());
#endif
    break;
}
