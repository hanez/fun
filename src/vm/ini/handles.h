/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-11-30
 */

/** INI handle registry for iniparser 4.2.6 */
#pragma once

#ifdef FUN_WITH_INI
#if defined(__has_include)
#  if __has_include(<iniparser/iniparser.h>)
#    include <iniparser/iniparser.h>
#    include <iniparser/dictionary.h>
#  elif __has_include(<iniparser.h>)
#    include <iniparser.h>
#    include <dictionary.h>
#  else
#    error "iniparser headers not found"
#  endif
#else
#  include <iniparser/iniparser.h>
#  include <iniparser/dictionary.h>
#endif
#include <stdio.h> /* snprintf for helper */

typedef struct { dictionary *dict; int in_use; } IniSlot;
static IniSlot g_ini[64];

static int ini_alloc_handle(dictionary *d) {
    for (int i = 1; i < (int)(sizeof(g_ini)/sizeof(g_ini[0])); ++i) {
        if (!g_ini[i].in_use) { g_ini[i].in_use = 1; g_ini[i].dict = d; return i; }
    }
    return 0;
}
static dictionary* ini_get(int h) {
    if (h > 0 && h < (int)(sizeof(g_ini)/sizeof(g_ini[0])) && g_ini[h].in_use) return g_ini[h].dict;
    return NULL;
}
static int ini_free_handle(int h) {
    if (h <= 0 || h >= (int)(sizeof(g_ini)/sizeof(g_ini[0])) || !g_ini[h].in_use) return 0;
    if (g_ini[h].dict) iniparser_freedict(g_ini[h].dict);
    g_ini[h].dict = NULL;
    g_ini[h].in_use = 0;
    return 1;
}

/* Helper to build section:key string safely into provided buffer */
static inline void ini_make_full_key(char *buf, size_t cap, const char *sec, const char *key) {
    if (!buf || cap == 0) return;
    if (!sec) sec = "";
    if (!key) key = "";
    /* iniparser expects "section:key" */
    snprintf(buf, cap, "%s:%s", sec, key);
}
#endif /* FUN_WITH_INI */
