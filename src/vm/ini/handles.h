/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file handles.h
 * @brief INI handle registry for iniparser 4.2.6 used by INI VM opcodes.
 */
#pragma once

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
#include <stddef.h>

/**
 * @brief One slot in the global INI handle registry.
 * @details Associates an iniparser dictionary pointer with an in-use flag.
 */
typedef struct {
  dictionary *dict;
  int in_use;
} IniSlot;

/** Single global registry (defined in handles.c). */
extern IniSlot g_ini[64];

/**
 * @brief Allocate a registry handle for a newly created dictionary.
 * @param d Pointer to an iniparser dictionary.
 * @return Handle id (>0) on success or 0 on failure.
 */
int ini_alloc_handle(dictionary *d);
/**
 * @brief Look up a dictionary pointer by registry handle.
 * @param h Handle id previously returned by ini_alloc_handle().
 * @return Pointer to dictionary or NULL if not found.
 */
dictionary *ini_get(int h);
/**
 * @brief Free a previously allocated handle and close its dictionary.
 * @param h Handle id to free.
 * @return 1 on success, 0 on error (invalid handle or not in use).
 */
int ini_free_handle(int h);

/**
 * @brief Build a fully qualified key "section:key" into a caller-provided buffer.
 * @param buf Destination buffer.
 * @param cap Capacity of buf in bytes (including terminator).
 * @param sec Section name (may be NULL for default section).
 * @param key Key name (must not be NULL).
 */
void ini_make_full_key(char *buf, size_t cap, const char *sec, const char *key);
#endif /* FUN_WITH_INI */
