/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file pcre2.c
 * @brief PCRE2 helpers for Fun VM extension opcodes (conditional build).
 *
 * This module centralizes the concrete PCRE2 implementation so VM opcodes in
 * src/vm/pcre2/*.c only perform stack marshalling and delegate to these
 * helpers. This mirrors the approach used by other extensions (e.g. SQLite,
 * XML2) where the heavy lifting lives under src/extensions/ and the opcodes
 * just call into small C helpers.
 *
 * Build-time feature flag:
 * - The code in this file is compiled only when FUN_WITH_PCRE2 is enabled.
 *   When disabled, PCRE2-dependent opcodes are built with no-op fallbacks.
 *
 * PCRE2 width configuration:
 * - PCRE2 requires defining PCRE2_CODE_UNIT_WIDTH before including <pcre2.h> to
 *   select 8/16/32-bit code units. We select 8-bit here. Because the Fun VM
 *   translates many opcode .c files into the same translation unit, it is
 *   important this macro is defined exactly once before the first <pcre2.h>
 *   inclusion. This file ensures that when FUN_WITH_PCRE2 is enabled.
 *
 * Flags mapping used by helpers/opcodes (bitmask in the VM):
 * - 1  -> PCRE2_CASELESS ("i")
 * - 2  -> PCRE2_MULTILINE ("m")
 * - 4  -> PCRE2_DOTALL ("s")
 * - 8  -> PCRE2_UTF ("u")
 * - 16 -> PCRE2_EXTENDED ("x")
 *
 * Value and memory ownership:
 * - The Value type and helper functions (make_map_empty, make_array_*, map_set,
 *   array_push, make_int, make_string, make_nil, string_substr, etc.) are
 *   provided by the Fun VM and are declared in the including translation unit.
 *   The arrays/maps returned from this module are owned by the caller (the
 *   VM opcode), consistent with other extension helpers.
 *
 * Thread-safety:
 * - These helpers are not inherently thread-safe, but they do not maintain any
 *   internal state beyond stack-local variables. Coordinate usage externally if
 *   the embedding is multi-threaded.
 */

/* Ensure PCRE2 is configured consistently across the whole translation unit.
 * vm.c includes many opcode implementation .c files; some use PCRE2. For PCRE2
 * headers to expose the correct typedefs (e.g., pcre2_code, PCRE2_SPTR), the
 * PCRE2_CODE_UNIT_WIDTH macro must be defined before the first inclusion of
 * <pcre2.h>. We do this once here when PCRE2 support is enabled. */
#ifdef FUN_WITH_PCRE2
#ifndef PCRE2_CODE_UNIT_WIDTH
#define PCRE2_CODE_UNIT_WIDTH 8
#endif
#include <pcre2.h>
#include <string.h>

/**
 * @brief Map Fun VM regex flags to PCRE2 compile options.
 *
 * The Fun VM passes a small integer bitmask controlling common regex
 * behaviours. This function translates those bits into the corresponding
 * PCRE2 compile options.
 *
 * Bit mapping:
 * - 1  -> PCRE2_CASELESS (case-insensitive)
 * - 2  -> PCRE2_MULTILINE (^ and $ match start/end of line)
 * - 4  -> PCRE2_DOTALL (dot matches newlines)
 * - 8  -> PCRE2_UTF (treat pattern/subject as UTF-8)
 * - 16 -> PCRE2_EXTENDED (ignore unescaped whitespace and allow comments)
 *
 * @param flags Bitmask provided by the VM.
 * @return uint32_t PCRE2 options suitable for pcre2_compile().
 */
static uint32_t fun_pcre2_opts_from_flags(int flags) {
  uint32_t opt = 0;
  if (flags & 1)  opt |= PCRE2_CASELESS;  /* I */
  if (flags & 2)  opt |= PCRE2_MULTILINE; /* M */
  if (flags & 4)  opt |= PCRE2_DOTALL;    /* S */
  if (flags & 8)  opt |= PCRE2_UTF;       /* U */
  if (flags & 16) opt |= PCRE2_EXTENDED;  /* X */
  return opt;
}

/**
 * @brief Test whether a pattern matches a subject at least once.
 *
 * Compiles the given pattern with options derived from the flags bitmask and
 * runs pcre2_match() once starting at offset 0.
 *
 * @param pattern NUL-terminated regex pattern string.
 * @param subject NUL-terminated subject string.
 * @param flags   VM bitmask translated by fun_pcre2_opts_from_flags().
 * @return int 1 if pcre2_match() returns a non-negative value; 0 if there is
 *             no match or an error occurs (including compile error or OOM).
 *
 * @note This helper performs only a single match attempt at offset 0; it does
 *       not search for subsequent matches. Use fun_pcre2_findall() for that.
 */
static int fun_pcre2_test(const char *pattern, const char *subject, int flags) {
  if (!pattern || !subject) return 0;
  int errorcode = 0; PCRE2_SIZE erroff = 0;
  uint32_t opt = fun_pcre2_opts_from_flags(flags);
  pcre2_code *re = pcre2_compile((PCRE2_SPTR)pattern, PCRE2_ZERO_TERMINATED, opt, &errorcode, &erroff, NULL);
  if (!re) return 0;
  pcre2_match_data *mdata = pcre2_match_data_create_from_pattern(re, NULL);
  int rc = pcre2_match(re, (PCRE2_SPTR)subject, (PCRE2_SIZE)strlen(subject), 0, 0, mdata, NULL);
  pcre2_match_data_free(mdata);
  pcre2_code_free(re);
  return rc >= 0 ? 1 : 0;
}

/* Build a Value API is provided by the VM; declarations are in the including TU. */
/**
 * @brief Match a pattern once and return a structured result map.
 *
 * On success, returns a map with the following keys:
 * - "full"   -> string: the matched substring for group 0
 * - "start"  -> int:    start index (0-based) of the match in the subject
 * - "end"    -> int:    end index (exclusive)
 * - "groups" -> array:  strings for each captured group (1..n), empty if none
 *
 * On no match, pattern compile failure, or memory allocation error, returns
 * Nil.
 *
 * @param pattern NUL-terminated regex pattern string.
 * @param subject NUL-terminated subject string.
 * @param flags   VM bitmask translated by fun_pcre2_opts_from_flags().
 * @return Value A VM map Value as described above, or Nil on failure.
 *
 * @see fun_pcre2_findall()
 */
static Value fun_pcre2_match(const char *pattern, const char *subject, int flags) {
  if (!pattern || !subject) return make_nil();
  int errorcode = 0; PCRE2_SIZE erroff = 0;
  uint32_t opt = fun_pcre2_opts_from_flags(flags);
  pcre2_code *re = pcre2_compile((PCRE2_SPTR)pattern, PCRE2_ZERO_TERMINATED, opt, &errorcode, &erroff, NULL);
  if (!re) return make_nil();
  pcre2_match_data *mdata = pcre2_match_data_create_from_pattern(re, NULL);
  int rc = pcre2_match(re, (PCRE2_SPTR)subject, (PCRE2_SIZE)strlen(subject), 0, 0, mdata, NULL);
  if (rc <= 0) {
    pcre2_match_data_free(mdata);
    pcre2_code_free(re);
    return make_nil();
  }
  PCRE2_SIZE *ov = pcre2_get_ovector_pointer(mdata);
  Value res = make_map_empty();
  int start0 = (int)ov[0];
  int end0 = (int)ov[1];
  char *full = string_substr(subject, start0, end0 - start0);
  (void)map_set(&res, "full", make_string(full ? full : ""));
  if (full) free(full);
  (void)map_set(&res, "start", make_int(start0));
  (void)map_set(&res, "end", make_int(end0));
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
  pcre2_match_data_free(mdata);
  pcre2_code_free(re);
  return res;
}

/**
 * @brief Find all non-overlapping matches of a pattern in a subject.
 *
 * Scans the subject from left to right and appends, for each non-overlapping
 * match, a map with the same shape as fun_pcre2_match() to the result array.
 * If the engine reports an empty match (start == end), the scan advances by a
 * single code unit to prevent infinite loops.
 *
 * On pattern compile failure or allocation error, returns an empty array.
 *
 * @param pattern NUL-terminated regex pattern string.
 * @param subject NUL-terminated subject string.
 * @param flags   VM bitmask translated by fun_pcre2_opts_from_flags().
 * @return Value An array of match maps; may be empty when no matches are found
 *               or on error.
 *
 * @see fun_pcre2_match()
 */
static Value fun_pcre2_findall(const char *pattern, const char *subject, int flags) {
  Value out = make_array_from_values(NULL, 0);
  if (!pattern || !subject) return out;
  int errorcode = 0; PCRE2_SIZE erroff = 0;
  uint32_t opt = fun_pcre2_opts_from_flags(flags);
  pcre2_code *re = pcre2_compile((PCRE2_SPTR)pattern, PCRE2_ZERO_TERMINATED, opt, &errorcode, &erroff, NULL);
  if (!re) return out;
  pcre2_match_data *mdata = pcre2_match_data_create_from_pattern(re, NULL);
  size_t subj_len = strlen(subject);
  size_t start_off = 0;
  while (1) {
    int rc = pcre2_match(re, (PCRE2_SPTR)subject, (PCRE2_SIZE)subj_len, start_off, 0, mdata, NULL);
    if (rc <= 0) break;
    PCRE2_SIZE *ov = pcre2_get_ovector_pointer(mdata);
    int s0 = (int)ov[0];
    int e0 = (int)ov[1];
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
    /* Advance safely (guard against empty match). */
    if (e0 == s0) {
      if ((size_t)e0 < subj_len) start_off = e0 + 1; else break;
    } else {
      start_off = e0;
    }
  }
  pcre2_match_data_free(mdata);
  pcre2_code_free(re);
  return out;
}
#endif /* FUN_WITH_PCRE2 */
