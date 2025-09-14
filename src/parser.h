/*
 * Parse a .fun source file and compile it into entry bytecode.
 * Minimal support:
 *  - Optional shebang on the first line.
 *  - Optional single function wrapper: fun <ident>() { ... }
 *  - print("...") statements inside function or at top-level.
 * Produces bytecode that executes all discovered print statements and halts.
 *
 * Returns: newly allocated Bytecode*, or NULL on error.
 */

#ifndef FUN_PARSER_H
#define FUN_PARSER_H

#include "bytecode.h"

Bytecode *parse_file_to_bytecode(const char *path);

/* Parse source provided as a single string buffer (for REPL, tests, etc.). */
Bytecode *parse_string_to_bytecode(const char *source);

#endif
