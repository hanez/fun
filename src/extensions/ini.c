/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file ini.c
 * @brief iniparser helpers for Fun VM INI-related opcodes (conditional build).
 *
 * This module centralizes small utilities and a tiny handle registry used by
 * VM opcodes that interact with INI configuration files through the
 * iniparser library. Placing the concrete logic in src/extensions/ keeps the
 * opcode implementations minimal — they focus on VM stack marshalling and
 * delegate the concrete work here, mirroring other extensions (PCRE2, SQLite, XML2).
 *
 * Build-time feature flag:
 * - All code in this file is compiled only when FUN_WITH_INI is enabled.
 *   When disabled, INI-related opcodes should provide safe no-op fallbacks
 *   in their respective VM files.
 *
 * Registry and ownership model:
 * - A very small fixed-size registry (g_ini) maps small positive integers to
 *   iniparser dictionaries. The registry OWNS the dictionary pointer and will
 *   call iniparser_freedict() when a handle is freed via ini_free_handle().
 * - Handles are in the range [1, 63]; 0 indicates failure/invalid.
 * - The registry does not perform I/O; callers are responsible for creating
 *   the dictionary (e.g., iniparser_load()) prior to registration.
 *
 * Thread-safety:
 * - Not thread-safe. If used from multiple threads, coordinate access
 *   externally.
 *
 * Key formatting helper:
 * - ini_make_full_key() produces a section-qualified key of the form
 *   "section:key", which is what iniparser expects for lookups.
 */

#ifdef FUN_WITH_INI
#if defined(__has_include)
#if __has_include(<iniparser/iniparser.h>)
#include <iniparser/dictionary.h>
#include <iniparser/iniparser.h>
#elif __has_include(<iniparser.h>)
#include <dictionary.h>
#include <iniparser.h>
#else
#error "iniparser headers not found"
#endif
#else
#include <iniparser/dictionary.h>
#include <iniparser/iniparser.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

/**
 * @brief One slot in the global INI handle registry.
 *
 * Associates an iniparser dictionary pointer with an in-use flag. The
 * registry takes ownership of the dictionary for the lifetime of the slot and
 * will free it when the handle is released via ini_free_handle().
 */
typedef struct {
  dictionary *dict;
  int in_use;
} IniSlot;

/**
 * @brief Fixed-size registry of iniparser dictionaries.
 *
 * Index 0 is reserved and never used for valid handles. Valid handles are in
 * the range [1, 63]. When an entry's in_use flag is 0, the slot is available.
 */
IniSlot g_ini[64];

/**
 * @brief Allocate a registry handle for a newly created dictionary.
 *
 * Transfers ownership of the dictionary pointer to the registry on success.
 *
 * @param d Pointer to an initialized iniparser dictionary (e.g., from
 *          iniparser_load()). Must not be NULL.
 * @return int Positive handle (>0) on success; 0 if no free slot is available
 *             or if d is NULL.
 */
int ini_alloc_handle(dictionary *d) {
  if (!d) return 0;
  for (int i = 1; i < (int)(sizeof(g_ini) / sizeof(g_ini[0])); ++i) {
    if (!g_ini[i].in_use) {
      g_ini[i].in_use = 1;
      g_ini[i].dict = d;
      return i;
    }
  }
  return 0;
}

/**
 * @brief Look up a dictionary pointer by registry handle.
 *
 * The returned pointer is owned by the registry; callers must not free it
 * directly. Use ini_free_handle() to release the association.
 *
 * @param h Handle id previously returned by ini_alloc_handle().
 * @return dictionary* Pointer to dictionary if the handle is valid and in use;
 *                     NULL otherwise.
 */
dictionary *ini_get(int h) {
  if (h > 0 && h < (int)(sizeof(g_ini) / sizeof(g_ini[0])) && g_ini[h].in_use) return g_ini[h].dict;
  return NULL;
}

/**
 * @brief Free a previously allocated handle and close its dictionary.
 *
 * If the slot holds a dictionary, iniparser_freedict() is called. The slot is
 * then marked available for reuse.
 *
 * @param h Handle id to free.
 * @return int 1 on success; 0 if the handle is invalid or not in use.
 */
int ini_free_handle(int h) {
  if (h <= 0 || h >= (int)(sizeof(g_ini) / sizeof(g_ini[0])) || !g_ini[h].in_use) return 0;
  if (g_ini[h].dict) iniparser_freedict(g_ini[h].dict);
  g_ini[h].dict = NULL;
  g_ini[h].in_use = 0;
  return 1;
}

/**
 * @brief Build a fully qualified key of the form "section:key".
 *
 * Writes into a caller-provided buffer a key qualified by section, as expected
 * by iniparser lookups. If sec is NULL, an empty section is used. If key is
 * NULL, an empty key is used. The function is a no-op if buf is NULL or cap is
 * 0. Output is always NUL-terminated (subject to snprintf semantics).
 *
 * @param buf Destination buffer.
 * @param cap Capacity of buf in bytes (including terminator).
 * @param sec Section name (may be NULL for default/empty section).
 * @param key Key name (may be NULL to produce an empty key).
 */
void ini_make_full_key(char *buf, size_t cap, const char *sec, const char *key) {
  if (!buf || cap == 0) return;
  if (!sec) sec = "";
  if (!key) key = "";
  /* iniparser expects section:key; lookup is case-insensitive internally */
  snprintf(buf, cap, "%s:%s", sec, key);
}

#endif /* FUN_WITH_INI */
