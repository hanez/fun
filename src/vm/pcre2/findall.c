/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file findall.c
 * @brief Implements the OP_PCRE2_FINDALL opcode (conditional build).
 *
 * Finds all non-overlapping matches of a PCRE2 pattern in a subject string
 * and returns an array of maps describing each match when FUN_WITH_PCRE2 is
 * enabled. When PCRE2 support is disabled at build time, the opcode falls
 * back to returning an empty array.
 */

/**
 * OP_PCRE2_FINDALL: (pattern:any, text:any, flags:int|bool=0) -> array(map)
 *
 * Behavior when FUN_WITH_PCRE2 is enabled:
 * - Pops three arguments from the VM stack: pattern, text, flags.
 *   - pattern and text are converted to strings using value_to_string_alloc().
 *   - flags bits map to PCRE2 options:
 *     - 1 = PCRE2_CASELESS (I)
 *     - 2 = PCRE2_MULTILINE (M)
 *     - 4 = PCRE2_DOTALL (S)
 *     - 8 = PCRE2_UTF (U)
 *     - 16 = PCRE2_EXTENDED (X)
 * - Compiles the pattern and scans the subject for all non-overlapping
 *   matches. For each match, pushes into the result array a map with keys:
 *   - "full": matched substring (group 0)
 *   - "start": start index (int)
 *   - "end": end index (int, exclusive)
 *   - "groups": array of captured group strings (excluding group 0)
 * - On compilation error or allocation failure, returns an empty array.
 *
 * Behavior when FUN_WITH_PCRE2 is disabled:
 * - Pops three values and returns an empty array.
 */

/* PCRE2_FINDALL */
case OP_PCRE2_FINDALL: {
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
    push_value(vm, make_array_from_values(NULL, 0));
    break;
  }
  Value out = fun_pcre2_findall(pattern, subject, flags);
  free(pattern);
  free(subject);
  push_value(vm, out);
#else
  Value a = pop_value(vm);
  free_value(a);
  Value b = pop_value(vm);
  free_value(b);
  Value c = pop_value(vm);
  free_value(c);
  push_value(vm, make_array_from_values(NULL, 0));
#endif
  break;
}
