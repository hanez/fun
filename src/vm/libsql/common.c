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

#ifdef FUN_WITH_LIBSQL
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sqlite3.h> /* libsql provides a sqlite3-compatible C API */

typedef struct LibSqlHandle {
    int id;
    sqlite3 *db;
    struct LibSqlHandle *next;
} LibSqlHandle;

static LibSqlHandle *g_libsql_handles = NULL;
static int g_libsql_next_id = 1;

static LibSqlHandle *libsql_reg_add(sqlite3 *db) {
    LibSqlHandle *h = (LibSqlHandle*)malloc(sizeof(LibSqlHandle));
    if (!h) return NULL;
    h->id = g_libsql_next_id++;
    h->db = db;
    h->next = g_libsql_handles;
    g_libsql_handles = h;
    return h;
}

static LibSqlHandle *libsql_reg_get(int id) {
    LibSqlHandle *p = g_libsql_handles;
    while (p) { if (p->id == id) return p; p = p->next; }
    return NULL;
}

static void libsql_reg_del(int id) {
    LibSqlHandle **pp = &g_libsql_handles;
    while (*pp) {
        if ((*pp)->id == id) {
            LibSqlHandle *dead = *pp;
            *pp = (*pp)->next;
            free(dead);
            return;
        }
        pp = &((*pp)->next);
    }
}
#endif /* FUN_WITH_LIBSQL */
