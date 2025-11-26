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
 * OP_LIBSQL_EXEC: (handle:int, sql:string) -> int rc (0=OK)
 */
case OP_LIBSQL_EXEC: {
#ifdef FUN_WITH_LIBSQL
    Value vsql = pop_value(vm);
    Value vh = pop_value(vm);
    int hid = (int)vh.i;
    char *sql = value_to_string_alloc(&vsql);
    free_value(vh);
    free_value(vsql);
    LibSqlHandle *h = libsql_reg_get(hid);
    if (!h || !h->db || !sql) { if (sql) free(sql); push_value(vm, make_int(SQLITE_MISUSE)); break; }
    char *errmsg = NULL;
    int rc = sqlite3_exec(h->db, sql, NULL, NULL, &errmsg);
    if (errmsg) sqlite3_free(errmsg);
    free(sql);
    push_value(vm, make_int(rc));
#else
    Value v1 = pop_value(vm); free_value(v1);
    Value v2 = pop_value(vm); free_value(v2);
    push_value(vm, make_int(-1));
#endif
    break;
}
