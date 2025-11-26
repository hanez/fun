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
 * OP_LIBSQL_QUERY: (handle:int, sql:string) -> array<map<string,any>>
 */
case OP_LIBSQL_QUERY: {
#ifdef FUN_WITH_LIBSQL
    Value vsql = pop_value(vm);
    Value vh = pop_value(vm);
    int hid = (int)vh.i;
    char *sql = value_to_string_alloc(&vsql);
    free_value(vh);
    free_value(vsql);
    LibSqlHandle *h = libsql_reg_get(hid);
    if (!h || !h->db || !sql) { if (sql) free(sql); push_value(vm, make_array_from_values(NULL, 0)); break; }
    sqlite3_stmt *stmt = NULL;
    if (sqlite3_prepare_v2(h->db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        free(sql);
        push_value(vm, make_array_from_values(NULL, 0));
        break;
    }
    free(sql);
    Value rows = make_array_from_values(NULL, 0);
    int ncols = sqlite3_column_count(stmt);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Value row = make_map_empty();
        for (int i = 0; i < ncols; i++) {
            const char *name = sqlite3_column_name(stmt, i);
            int type = sqlite3_column_type(stmt, i);
            Value kv;
            switch (type) {
                case SQLITE_INTEGER: kv = make_int((int64_t)sqlite3_column_int64(stmt, i)); break;
                case SQLITE_FLOAT:   kv = make_float(sqlite3_column_double(stmt, i)); break;
                case SQLITE_TEXT:    kv = make_string((const char*)sqlite3_column_text(stmt, i)); break;
                case SQLITE_NULL:    kv = make_nil(); break;
                default:             kv = make_nil(); break; /* ignore blobs for now */
            }
            (void)map_set(&row, name ? name : "", kv);
        }
        (void)array_push(&rows, row);
        /* Do NOT free 'row' here; owned by rows array. */
    }
    sqlite3_finalize(stmt);
    push_value(vm, rows);
#else
    Value v1 = pop_value(vm); free_value(v1);
    Value v2 = pop_value(vm); free_value(v2);
    push_value(vm, make_array_from_values(NULL, 0));
#endif
    break;
}
