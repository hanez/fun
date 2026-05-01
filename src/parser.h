/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file parser.h
 * @brief Public API for parsing Fun source into bytecode.
 */

#ifndef FUN_PARSER_H
#define FUN_PARSER_H

#include "bytecode.h"

/**
 * @brief Parse a .fun source file and compile it into a bytecode chunk.
 *
 * The parser accepts an optional shebang on the first line and supports a
 * minimal top-level or single-function program model. The returned bytecode
 * will execute discovered statements (e.g., print) and then halt.
 *
 * @param path Filesystem path to the .fun source file. Must be a
 *             null-terminated UTF-8 string.
 * @return Newly allocated Bytecode instance on success, or NULL on error.
 */
Bytecode *parse_file_to_bytecode(const char *path);

/**
 * @brief Parse source from a provided string buffer (REPL/tests helper).
 *
 * @param source Null-terminated Fun program text.
 * @return Newly allocated Bytecode instance on success, or NULL on error.
 */
Bytecode *parse_string_to_bytecode(const char *source);

/**
 * @brief Retrieve information about the last parser error, if any.
 *
 * @param msgBuf  Output buffer to receive a human-readable error message.
 * @param msgCap  Capacity of msgBuf in bytes.
 * @param outLine Optional output: 1-based line number where the error occurred.
 * @param outCol  Optional output: 1-based column number where the error occurred.
 * @return 1 if an error was present and fields were populated, 0 if there is
 *         no recorded error.
 */
int parser_last_error(char *msgBuf, unsigned long msgCap, int *outLine, int *outCol);

#endif
