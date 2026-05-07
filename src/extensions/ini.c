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
 * @brief INI parsing helpers and VM opcode support via iniparser.
 *
 * Provides includes and declarations required for INI-related opcodes when
 * FUN_WITH_INI is enabled at build time.
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
 * @details Associates an iniparser dictionary pointer with an in-use flag.
 */
typedef struct {
  dictionary *dict;
  int in_use;
} IniSlot;

IniSlot g_ini[64];

/**
 * @brief Allocate a registry handle for a newly created dictionary.
 * @param d Pointer to an iniparser dictionary.
 * @return Handle id (>0) on success or 0 on failure.
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
 * @param h Handle id previously returned by ini_alloc_handle().
 * @return Pointer to dictionary or NULL if not found.
 */
dictionary *ini_get(int h) {
  if (h > 0 && h < (int)(sizeof(g_ini) / sizeof(g_ini[0])) && g_ini[h].in_use) return g_ini[h].dict;
  return NULL;
}

/**
 * @brief Free a previously allocated handle and close its dictionary.
 * @param h Handle id to free.
 * @return 1 on success, 0 on error (invalid handle or not in use).
 */
int ini_free_handle(int h) {
  if (h <= 0 || h >= (int)(sizeof(g_ini) / sizeof(g_ini[0])) || !g_ini[h].in_use) return 0;
  if (g_ini[h].dict) iniparser_freedict(g_ini[h].dict);
  g_ini[h].dict = NULL;
  g_ini[h].in_use = 0;
  return 1;
}

/**
 * @brief Build a fully qualified key "section:key" into a caller-provided buffer.
 * @param buf Destination buffer.
 * @param cap Capacity of buf in bytes (including terminator).
 * @param sec Section name (may be NULL for default section).
 * @param key Key name (must not be NULL).
 */
void ini_make_full_key(char *buf, size_t cap, const char *sec, const char *key) {
  if (!buf || cap == 0) return;
  if (!sec) sec = "";
  if (!key) key = "";
  /* iniparser expects section:key; lookup is case-insensitive internally */
  snprintf(buf, cap, "%s:%s", sec, key);
}

#endif /* FUN_WITH_INI */
