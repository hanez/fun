/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file regex_match.c
 * @brief VM opcode snippet for OP_REGEX_MATCH (POSIX full-match).
 *
 * This opcode checks whether a regular expression pattern matches the entire
 * input string. It uses POSIX regex APIs on UNIX platforms and provides a
 * graceful fallback elsewhere.
 *
 * Behavior (stack effects):
 * - Pops: pattern (string), input (string)
 * - Pushes: result (int) — 1 if the pattern matches the whole input string,
 *   0 otherwise. On invalid regex, returns 0.
 *
 * Platform notes:
 * - On non-UNIX platforms (no POSIX regex available), this opcode returns 0
 *   without error.
 *
 * Errors:
 * - If operands are not strings, the VM prints a runtime type error and exits.
 *
 * Example:
 * - pattern = "[a-z]+", input = "hello"  -> 1
 * - pattern = "[a-z]+", input = "hello!" -> 0 (not a full match)
 */
/* Regex full-match opcode using POSIX regex */
#ifdef __unix__
#include <regex.h>
#endif

case OP_REGEX_MATCH: {
  Value pattern = pop_value(vm);
  Value str = pop_value(vm);
  if (str.type != VAL_STRING || pattern.type != VAL_STRING) {
    fprintf(stderr, "Runtime type error: REGEX_MATCH expects (string, string)\n");
    exit(1);
  }
#ifndef __unix__
  /* Not supported on non-UNIX: return 0 gracefully */
  free_value(pattern);
  int truth = 0;
  free_value(str);
  push_value(vm, make_int(truth));
  break;
#else
  regex_t rx;
  int rc = regcomp(&rx, pattern.s ? pattern.s : "", REG_EXTENDED);
  if (rc != 0) {
    /* invalid regex -> false */
    free_value(pattern);
    free_value(str);
    push_value(vm, make_int(0));
    break;
  }
  regmatch_t m;
  int ok = regexec(&rx, str.s ? str.s : "", 1, &m, 0) == 0;
  int truth = 0;
  if (ok) {
    /* full match means the match spans whole string */
    if (m.rm_so == 0 && str.s) {
      size_t slen = strlen(str.s);
      truth = (m.rm_eo == (regoff_t)slen) ? 1 : 0;
    }
  }
  regfree(&rx);
  free_value(pattern);
  free_value(str);
  push_value(vm, make_int(truth));
  break;
#endif
}
