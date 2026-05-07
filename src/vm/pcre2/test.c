/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file test.c
 * @brief Implements the OP_PCRE2_TEST opcode (conditional build).
 *
 * Tests whether a PCRE2 pattern matches a subject string and returns 1 on
 * success or 0 otherwise when FUN_WITH_PCRE2 is enabled. When PCRE2 support
 * is disabled at build time, the opcode always returns 0.
 */

/**
 * OP_PCRE2_TEST: (pattern:any, text:any, flags:int|bool=0) -> int
 *
 * Behavior when FUN_WITH_PCRE2 is enabled:
 * - Pops: pattern, text, flags. Converts pattern/text to strings.
 * - Flags bits:
 *   - 1 = PCRE2_CASELESS (I)
 *   - 2 = PCRE2_MULTILINE (M)
 *   - 4 = PCRE2_DOTALL (S)
 *   - 8 = PCRE2_UTF (U)
 *   - 16 = PCRE2_EXTENDED (X)
 * - Returns 1 if pcre2_match() returns a non-negative result, else 0.
 * - On compile error or allocation failure, returns 0.
 *
 * Behavior when FUN_WITH_PCRE2 is disabled:
 * - Pops three values and returns 0.
 */

/* PCRE2_TEST */
case OP_PCRE2_TEST: {
#ifdef FUN_WITH_PCRE2
  Value vflags = pop_value(vm);
  Value vtext = pop_value(vm);
  Value vpat = pop_value(vm);
  int flags = 0;
  if (vflags.type == VAL_INT || vflags.type == VAL_BOOL) flags = (int)vflags.i;
  char *pattern = value_to_string_alloc(&vpat);
  char *subject = value_to_string_alloc(&vtext);
  free_value(vflags);
  free_value(vtext);
  free_value(vpat);
  if (!pattern || !subject) {
    if (pattern) free(pattern);
    if (subject) free(subject);
    push_value(vm, make_int(0));
    break;
  }
  int rc = fun_pcre2_test(pattern, subject, flags);
  free(pattern);
  free(subject);
  push_value(vm, make_int(rc));
#else
  /* pop args and return 0 when PCRE2 disabled */
  Value a = pop_value(vm);
  free_value(a);
  Value b = pop_value(vm);
  free_value(b);
  Value c = pop_value(vm);
  free_value(c);
  push_value(vm, make_int(0));
#endif
  break;
}
