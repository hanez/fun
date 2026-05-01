/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file regex_search.c
 * @brief VM opcode snippet for OP_REGEX_SEARCH (POSIX first-match search).
 *
 * Finds the first match of a POSIX regular expression within an input string
 * and returns a map with details about the match and captured groups.
 *
 * Behavior (stack effects):
 * - Pops: pattern (string), input (string)
 * - Pushes: map with keys:
 *   - "match": string — the matched substring (empty if no match)
 *   - "start": int — start index of the match, or -1
 *   - "end": int — end index (exclusive) of the match, or -1
 *   - "groups": array<string> — captured groups (may be empty)
 *
 * Platform notes:
 * - On non-UNIX platforms (no POSIX regex available), returns a default map
 *   with empty match, start=-1, end=-1, and empty groups.
 *
 * Errors:
 * - If operands are not strings, the VM prints a runtime type error and exits.
 * - If the pattern is an invalid regex, returns the default empty map.
 *
 * Example:
 * - pattern = "h(ell)o", input = "oh hello!" ->
 *   { match: "hello", start: 3, end: 8, groups: ["ell"] }
 */

/* Regex search (first match) opcode using POSIX regex */
#ifdef __unix__
#include <regex.h>
#include <stdlib.h>
#include <string.h>
#endif

case OP_REGEX_SEARCH: {
  Value pattern = pop_value(vm);
  Value str = pop_value(vm);
  if (str.type != VAL_STRING || pattern.type != VAL_STRING) {
    fprintf(stderr, "Runtime type error: REGEX_SEARCH expects (string, string)\n");
    exit(1);
  }
#ifndef __unix__
  /* Return default empty result on unsupported platforms */
  Value m = make_map_empty();
  (void)map_set(&m, "match", make_string(""));
  (void)map_set(&m, "start", make_int(-1));
  (void)map_set(&m, "end", make_int(-1));
  Value emptyArr = make_array_from_values(NULL, 0);
  (void)map_set(&m, "groups", emptyArr);
  free_value(pattern);
  free_value(str);
  push_value(vm, m);
  break;
#else
  regex_t rx;
  int rc = regcomp(&rx, pattern.s ? pattern.s : "", REG_EXTENDED);
  if (rc != 0) {
    /* invalid regex -> empty result */
    Value m = make_map_empty();
    (void)map_set(&m, "match", make_string(""));
    (void)map_set(&m, "start", make_int(-1));
    (void)map_set(&m, "end", make_int(-1));
    Value emptyArr = make_array_from_values(NULL, 0);
    (void)map_set(&m, "groups", emptyArr);
    free_value(pattern);
    free_value(str);
    push_value(vm, m);
    break;
  }
  /* capture up to, say, 10 groups (including whole match) */
  enum { MAX_CAP = 16 };
  regmatch_t caps[MAX_CAP];
  int ok = regexec(&rx, str.s ? str.s : "", MAX_CAP, caps, 0) == 0;
  Value outMap = make_map_empty();
  if (!ok) {
    (void)map_set(&outMap, "match", make_string(""));
    (void)map_set(&outMap, "start", make_int(-1));
    (void)map_set(&outMap, "end", make_int(-1));
    Value emptyArr = make_array_from_values(NULL, 0);
    (void)map_set(&outMap, "groups", emptyArr);
  } else {
    int s = (int)caps[0].rm_so;
    int e = (int)caps[0].rm_eo;
    char *matchStr = NULL;
    if (str.s && s >= 0 && e >= s) {
      int len = e - s;
      matchStr = (char *)malloc((size_t)len + 1);
      if (matchStr) {
        memcpy(matchStr, str.s + s, (size_t)len);
        matchStr[len] = '\0';
      }
    }
    (void)map_set(&outMap, "match", make_string(matchStr ? matchStr : ""));
    if (matchStr) free(matchStr);
    (void)map_set(&outMap, "start", make_int(s));
    (void)map_set(&outMap, "end", make_int(e));
    /* groups 1..n */
    Value groupsArr = make_array_from_values(NULL, 0);
    /* Count groups available */
    int groupCount = 0;
    for (int i = 1; i < MAX_CAP; ++i) {
      if (caps[i].rm_so == -1 || caps[i].rm_eo == -1) break;
      groupCount++;
    }
    if (groupCount > 0) {
      Value *vals = (Value *)calloc((size_t)groupCount, sizeof(Value));
      int vi = 0;
      for (int i = 1; i <= groupCount; ++i) {
        int gs = (int)caps[i].rm_so;
        int ge = (int)caps[i].rm_eo;
        char *gstr = NULL;
        if (str.s && gs >= 0 && ge >= gs) {
          int gl = ge - gs;
          gstr = (char *)malloc((size_t)gl + 1);
          if (gstr) {
            memcpy(gstr, str.s + gs, (size_t)gl);
            gstr[gl] = '\0';
          }
        }
        vals[vi++] = make_string(gstr ? gstr : "");
        if (gstr) free(gstr);
      }
      groupsArr = make_array_from_values(vals, groupCount);
      for (int i = 0; i < groupCount; ++i)
        free_value(vals[i]);
      free(vals);
    }
    (void)map_set(&outMap, "groups", groupsArr);
  }
  regfree(&rx);
  free_value(pattern);
  free_value(str);
  push_value(vm, outMap);
  break;
#endif
}
