/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file match.c
 * @brief Implements the OP_PCRE2_MATCH opcode (conditional build).
 *
 * Executes a single PCRE2 pattern match against a subject string and returns
 * a map describing the first match and its capture groups when FUN_WITH_PCRE2
 * is enabled. Returns Nil when there is no match or on error. When PCRE2
 * support is disabled at build time, the opcode always returns Nil.
 */

/**
 * OP_PCRE2_MATCH: (pattern:any, text:any, flags:int|bool=0) -> map|Nil
 *
 * Behavior when FUN_WITH_PCRE2 is enabled:
 * - Pops: pattern, text, flags. Converts pattern/text to strings.
 * - Flags bits:
 *   - 1 = PCRE2_CASELESS (I)
 *   - 2 = PCRE2_MULTILINE (M)
 *   - 4 = PCRE2_DOTALL (S)
 *   - 8 = PCRE2_UTF (U)
 *   - 16 = PCRE2_EXTENDED (X)
 * - On successful match, returns a map with keys:
 *   - "full": matched substring (group 0)
 *   - "start": start index (int)
 *   - "end": end index (int, exclusive)
 *   - "groups": array of captured group strings (excluding group 0)
 * - On no match or compile error, returns Nil.
 *
 * Behavior when FUN_WITH_PCRE2 is disabled:
 * - Pops three values and returns Nil.
 */

/* PCRE2_MATCH */
case OP_PCRE2_MATCH: {
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
    push_value(vm, make_nil());
    break;
  }
  Value res = fun_pcre2_match(pattern, subject, flags);
  free(pattern);
  free(subject);
  push_value(vm, res);
#else
  Value a = pop_value(vm);
  free_value(a);
  Value b = pop_value(vm);
  free_value(b);
  Value c = pop_value(vm);
  free_value(c);
  push_value(vm, make_nil());
#endif
  break;
}
