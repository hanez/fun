/*
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
 * This translation unit implements a tiny in-process registry for SQLite
 * connection handles that can be used by the VM opcodes living under
 * src/vm/sqlite/*.c. The registry abstracts over raw sqlite3* pointers and
 * assigns small positive integer identifiers to each connection. VM opcodes can
 * then pass these identifiers around instead of raw pointers.
 *
 * Build-time feature flag
 * -----------------------
 * The code is compiled only when the CMake option FUN_WITH_SQLITE is enabled
 * (i.e., the preprocessor symbol FUN_WITH_SQLITE is defined). When disabled,
 * this file contributes no symbols and the corresponding opcodes should be
 * compiled out or provide appropriate fallbacks.
 *
 * Ownership and lifetime
 * ----------------------
 * - The registry does NOT open or close databases. It merely stores pointers
 *   that were created elsewhere (e.g., via sqlite3_open()).
 * - Adding an entry does not transfer ownership of the sqlite3 connection.
 *   Callers remain responsible for invoking sqlite3_close() at the appropriate
 *   time.
 * - Removing an entry from the registry does NOT close the connection; it only
 *   forgets the mapping between id and pointer.
 * - Integer identifiers are monotonically increasing per process. Once a handle
 *   id is deleted, it will not be reused within the same process lifetime.
 *
 * Error handling
 * --------------
 * Functions in this module perform only basic validation and memory allocation.
 * Allocation failures return NULL (for lookups/additions) or are silently
 * ignored (for deletions of non-existent ids). No SQLite API calls are made
 * here, so no sqlite error codes are produced by this module itself.
 *
 * Thread-safety
 * -------------
 * The registry is implemented as a simple singly-linked list with no
 * synchronization. It is NOT thread-safe. If the VM uses SQLite from multiple
 * threads, the caller must provide external synchronization around all calls to
 * these helpers.
 *
 * Example
 * -------
 * @code{.c}
 * // Open a database elsewhere:
 * sqlite3 *db = NULL;
 * if (sqlite3_open(":memory:", &db) == SQLITE_OK) {
 *   // Register and get an id:
 *   SqlHandle *h = sql_reg_add(db);
 *   int id = h ? h->id : -1;
 *
 *   // Later look it up:
 *   SqlHandle *same = sql_reg_get(id);
 *   if (same) {
 *     // use same->db with SQLite APIs
 *   }
 *
 *   // When finished, drop the registry entry and close manually:
 *   sql_reg_del(id);
 *   sqlite3_close(db);
 * }
 * @endcode
 */
#ifdef FUN_WITH_SQLITE
#include <sqlite3.h>

/**
 * @brief Node in a singly-linked list of registered SQLite handles.
 *
 * Each node associates a monotonically increasing positive integer identifier
 * with a raw sqlite3* pointer. The list head is stored in a file-static global
 * (g_sql_handles).
 */
typedef struct SqlHandle {
  int id;                 /**< Positive identifier assigned by the registry. */
  sqlite3 *db;            /**< Opaque pointer to an opened sqlite3 connection. */
  struct SqlHandle *next; /**< Next entry in the singly-linked list. */
} SqlHandle;

/**
 * @brief Global head of the SQLite handle list.
 *
 * NULL denotes an empty list. The list is modified only by the helpers in this
 * file and is never exposed directly to callers.
 */
static SqlHandle *g_sql_handles = NULL;
/**
 * @brief Next positive identifier to assign to a newly added handle.
 *
 * Starts at 1 and increases monotonically. Identifiers are not reused across
 * deletions within a process lifetime.
 */
static int g_sql_next_id = 1;

/**
 * @brief Add a sqlite3 handle to the registry.
 *
 * Allocates a new list node, assigns a fresh positive id, and prepends it to
 * the internal registry list. Ownership of the sqlite3 connection remains with
 * the caller; this registry does not close the handle during deletion.
 *
 * @param db Valid pointer to an opened sqlite3 connection.
 * @return Pointer to the newly created SqlHandle entry on success; NULL on
 *         allocation failure. The returned pointer remains owned by the
 *         registry; do not free it directly.
 *
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
 * Performs a linear search over the internal list to find a matching id.
 *
 * @param id Positive identifier previously returned by sql_reg_add().
 * @return Pointer to the SqlHandle entry if found; NULL otherwise.
 *
 * @note The returned pointer is owned by the registry and must not be freed by
 *       the caller.
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
 *
 * @note This function does not close the underlying sqlite3 connection; the
 *       caller is responsible for calling sqlite3_close() if appropriate.
 * @note If the id does not exist, the function is a no-op.
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
