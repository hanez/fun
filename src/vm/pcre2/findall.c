/**
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
#ifndef PCRE2_CODE_UNIT_WIDTH
#define PCRE2_CODE_UNIT_WIDTH 8
#endif
#include <pcre2.h>
  int errorcode;
  PCRE2_SIZE erroff;
  uint32_t opt = 0;
  if (flags & 1) opt |= PCRE2_CASELESS;  /* I */
  if (flags & 2) opt |= PCRE2_MULTILINE; /* M */
  if (flags & 4) opt |= PCRE2_DOTALL;    /* S */
  if (flags & 8) opt |= PCRE2_UTF;       /* U */
  if (flags & 16) opt |= PCRE2_EXTENDED; /* X */
  pcre2_code *re = pcre2_compile((PCRE2_SPTR)pattern, PCRE2_ZERO_TERMINATED, opt, &errorcode, &erroff, NULL);
  if (!re) {
    free(pattern);
    free(subject);
    push_value(vm, make_array_from_values(NULL, 0));
    break;
  }
  pcre2_match_data *mdata = pcre2_match_data_create_from_pattern(re, NULL);
  Value out = make_array_from_values(NULL, 0);
  size_t subj_len = strlen(subject);
  size_t start_off = 0;
  int gcount = 0;
  while (1) {
    int rc = pcre2_match(re, (PCRE2_SPTR)subject, (PCRE2_SIZE)subj_len, start_off, 0, mdata, NULL);
    if (rc <= 0) break;
    PCRE2_SIZE *ov = pcre2_get_ovector_pointer(mdata);
    int s0 = (int)ov[0];
    int e0 = (int)ov[1];
    /* result map for this match */
    Value res = make_map_empty();
    char *full = string_substr(subject, s0, e0 - s0);
    (void)map_set(&res, "full", make_string(full ? full : ""));
    if (full) free(full);
    (void)map_set(&res, "start", make_int(s0));
    (void)map_set(&res, "end", make_int(e0));
    Value groups = make_array_from_values(NULL, 0);
    for (int i = 1; i < rc; ++i) {
      int s = (int)ov[2 * i];
      int e = (int)ov[2 * i + 1];
      char *gstr = (s >= 0 && e >= s) ? string_substr(subject, s, e - s) : NULL;
      Value gv = make_string(gstr ? gstr : "");
      if (gstr) free(gstr);
      (void)array_push(&groups, gv);
    }
    (void)map_set(&res, "groups", groups);
    (void)array_push(&out, res);
    /* advance start offset; guard against empty match */
    if (e0 == s0) {
      if ((size_t)e0 < subj_len) {
        start_off = e0 + 1;
      } else {
        break;
      }
    } else {
      start_off = e0;
    }
    gcount = rc;
  }
  pcre2_match_data_free(mdata);
  pcre2_code_free(re);
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
