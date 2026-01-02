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
#include <stddef.h>

typedef struct { dictionary *dict; int in_use; } IniSlot;

/* Single global registry (defined in handles.c) */
extern IniSlot g_ini[64];

/* Registry API (implemented in handles.c) */
int ini_alloc_handle(dictionary *d);
dictionary* ini_get(int h);
int ini_free_handle(int h);

/* Helper to build section:key string safely into provided buffer (implemented in handles.c) */
void ini_make_full_key(char *buf, size_t cap, const char *sec, const char *key);
#endif /* FUN_WITH_INI */
