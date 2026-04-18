/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-12-11 (2025-12-11 migrated from src/vm.c)
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
#include "vm/ini/handles.h"
#endif
