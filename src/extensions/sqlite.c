/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file sqlite.c
 * @brief SQLite handle registry and helper utilities for the Fun VM extension.
 *
 * This module provides a very small registry for SQLite database handles when
 * the project is compiled with FUN_WITH_SQLITE. It assigns small integer
 * identifiers to opened sqlite3 connections and allows retrieval or removal of
 * those entries. The registry itself does not open or close SQLite databases —
 * it only stores pointers provided by the caller.
 *
 * Thread-safety: This registry is not thread-safe. Callers must ensure
 * external synchronization if used from multiple threads.
 */
#ifdef FUN_WITH_SQLITE
#include <sqlite3.h>

/**
 * @brief Node in a singly-linked list of registered SQLite handles.
 */
typedef struct SqlHandle {
  int id;
  sqlite3 *db;
  struct SqlHandle *next;
} SqlHandle;

/** Global head of the SQLite handle list. */
static SqlHandle *g_sql_handles = NULL;
/** Next positive identifier to assign to a newly added handle. */
static int g_sql_next_id = 1;

/**
 * @brief Add a sqlite3 handle to the registry.
 *
 * The function allocates a new list node, assigns a fresh positive id, and
 * prepends it to the internal registry list.
 *
 * @param db Valid pointer to an opened sqlite3 connection.
 * @return Pointer to the newly created SqlHandle entry on success; NULL on
 *         allocation failure. The returned pointer remains owned by the
 *         registry; do not free it directly.
 * @note This function does not take ownership of the sqlite3 connection in the
 *       sense of closing it; removal from the registry will not call
 *       sqlite3_close().
 */
static SqlHandle *sql_reg_add(sqlite3 *db) {
  SqlHandle *h = (SqlHandle *)calloc(1, sizeof(SqlHandle));
  if (!h) return NULL;
  h->id = g_sql_next_id++;
  h->db = db;
  h->next = g_sql_handles;
  g_sql_handles = h;
  return h;
}

/**
 * @brief Look up a registered SQLite handle by id.
 *
 * @param id Positive identifier previously returned by sql_reg_add().
 * @return Pointer to the SqlHandle entry if found; NULL otherwise.
 */
static SqlHandle *sql_reg_get(int id) {
  for (SqlHandle *p = g_sql_handles; p; p = p->next)
    if (p->id == id) return p;
  return NULL;
}

/**
 * @brief Remove a SQLite handle entry from the registry.
 *
 * Deletes the list node associated with the given id.
 *
 * @param id Positive identifier of the entry to remove.
 * @note This function does not close the underlying sqlite3 connection; the
 *       caller is responsible for calling sqlite3_close() if appropriate.
 */
static void sql_reg_del(int id) {
  SqlHandle **pp = &g_sql_handles;
  while (*pp) {
    if ((*pp)->id == id) {
      SqlHandle *d = *pp;
      *pp = d->next;
      free(d);
      return;
    }
    pp = &(*pp)->next;
  }
}
#endif
