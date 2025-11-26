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
 * SQLite handle registry and helpers
 */
#ifdef FUN_WITH_SQLITE
#include <sqlite3.h>

typedef struct SqlHandle {
    int id;
    sqlite3 *db;
    struct SqlHandle *next;
} SqlHandle;

static SqlHandle *g_sql_handles = NULL;
static int g_sql_next_id = 1;

static SqlHandle* sql_reg_add(sqlite3 *db) {
    SqlHandle *h = (SqlHandle*)calloc(1, sizeof(SqlHandle));
    if (!h) return NULL;
    h->id = g_sql_next_id++;
    h->db = db;
    h->next = g_sql_handles;
    g_sql_handles = h;
    return h;
}

static SqlHandle* sql_reg_get(int id) {
    for (SqlHandle *p = g_sql_handles; p; p = p->next) if (p->id == id) return p;
    return NULL;
}

static void sql_reg_del(int id) {
    SqlHandle **pp = &g_sql_handles;
    while (*pp) {
        if ((*pp)->id == id) { SqlHandle *d = *pp; *pp = d->next; free(d); return; }
        pp = &(*pp)->next;
    }
}
#endif
