/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-01-03
 */

/* Common helpers/state for Notcurses TUI ops */
#pragma once

#include "value.h"

/* Forward declarations to avoid depending on the notcurses headers unless enabled */
struct notcurses;
struct ncplane;

/* Shared static state within vm.c translation unit (header is included into vm.c at file scope) */
static struct notcurses* _fun_nc = NULL;
static struct ncplane*   _fun_nc_std = NULL;
