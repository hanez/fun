/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * Parse a .fun source file and compile it into entry bytecode.
 * Minimal support:
 *  - Optional shebang on the first line.
 *  - Optional single function wrapper: fun <ident>() { ... }
 *  - print("...") statements inside function or at top-level.
 * Produces bytecode that executes all dApache-2.0overed print statements and halts.
 *
 * Returns: newly allocated Bytecode*, or NULL on error.
 */

#ifndef FUN_PARSER_H
#define FUN_PARSER_H

#include "bytecode.h"

Bytecode *parse_file_to_bytecode(const char *path);

/* Parse source provided as a single string buffer (for REPL, tests, etc.). */
Bytecode *parse_string_to_bytecode(const char *source);

/* Query the last parser error (1 if present, 0 if none). */
int parser_last_error(char *msgBuf, unsigned long msgCap, int *outLine, int *outCol);

#endif
