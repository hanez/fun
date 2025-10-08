/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-01
 */

// Extra string utilities

// Trim whitespace on the left (space, tab, CR, LF)
fun str_ltrim(s)
  src = to_string(s)
  number i = 0
  ws = " \t\r\n"
  while i < len(src)
    ch = substr(src, i, 1)
    if (find(ws, ch) < 0)
      break
    i = i + 1
  return substr(src, i, len(src) - i)

// Trim whitespace on the right
fun str_rtrim(s)
  src = to_string(s)
  number i = len(src) - 1
  ws = " \t\r\n"
  while i >= 0
    ch = substr(src, i, 1)
    if (find(ws, ch) < 0)
      break
    i = i - 1
  return substr(src, 0, i + 1)

// Trim both sides
fun str_trim(s)
  return str_rtrim(str_ltrim(s))

// Return 1 if s starts with prefix, else 0
fun str_starts_with(s, prefix)
  a = to_string(s)
  p = to_string(prefix)
  if (len(p) > len(a))
    return 0
  return substr(a, 0, len(p)) == p

// Return 1 if s ends with suffix, else 0
fun str_ends_with(s, suffix)
  a = to_string(s)
  p = to_string(suffix)
  number la = len(a)
  number lp = len(p)
  if (lp > la)
    return 0
  return substr(a, la - lp, lp) == p

// Split by a single-character delimiter, returns array of strings
fun str_split(s, delim)
  src = to_string(s)
  d = to_string(delim)
  // Use only the first character of delim
  if (len(d) == 0)
    return [src]
  dd = substr(d, 0, 1)
  parts = []
  buf = []
  number i = 0
  number n = len(src)
  while i < n
    ch = substr(src, i, 1)
    if (ch == dd)
      push(parts, join(buf, ""))
      buf = []
    else
      push(buf, ch)
    i = i + 1
  // tail
  push(parts, join(buf, ""))
  return parts

// Replace all occurrences of 'from' with 'to' (naive scan)
fun str_replace_all(s, from, to)
  src = to_string(s)
  f = to_string(from)
  t = to_string(to)
  number n = len(src)
  number lf = len(f)
  if (lf == 0)
    return src
  out = []
  number i = 0
  while i < n
    if (i + lf <= n) && (substr(src, i, lf) == f)
      push(out, t)
      i = i + lf
    else
      push(out, substr(src, i, 1))
      i = i + 1
  return join(out, "")

// Lowercase transform for ASCII A..Z
fun str_to_lower(s)
  src = to_string(s)
  U = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
  L = "abcdefghijklmnopqrstuvwxyz"
  out = []
  number i = 0
  number n = len(src)
  while i < n
    ch = substr(src, i, 1)
    idx = find(U, ch)
    if (idx >= 0)
      push(out, substr(L, idx, 1))
    else
      push(out, ch)
    i = i + 1
  return join(out, "")

// Uppercase transform for ASCII a..z
fun str_to_upper(s)
  src = to_string(s)
  U = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
  L = "abcdefghijklmnopqrstuvwxyz"
  out = []
  number i = 0
  number n = len(src)
  while i < n
    ch = substr(src, i, 1)
    idx = find(L, ch)
    if (idx >= 0)
      push(out, substr(U, idx, 1))
    else
      push(out, ch)
    i = i + 1
  return join(out, "")

// Repeat string s 'count' times
fun str_repeat(s, count)
  src = to_string(s)
  number c = count
  if (c <= 0)
    return ""
  parts = []
  number i = 0
  while i < c
    push(parts, src)
    i = i + 1
  return join(parts, "")

// ASCII string to bytes (printable ASCII 0x20..0x7E)
fun string_to_bytes_ascii(s)
  str = to_string(s)
  out = []
  number i = 0
  // ASCII printable ranges
  P1 = " !\"#$%&'()*+,-./"          // 32..47
  P2 = "0123456789"                 // 48..57
  P3 = ":;<=>?@"                    // 58..64
  P4 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ" // 65..90
  P5 = "[\\]^_`"                    // 91..96
  P6 = "abcdefghijklmnopqrstuvwxyz" // 97..122
  P7 = "{|}~"                       // 123..126
  while true
    ch = substr(str, i, 1)
    if (typeof(ch) != "String" || ch == "")
      break
    number code = -1
    idx = find(P1, ch)
    if (idx >= 0)
      code = 32 + idx
    else
      idx = find(P2, ch)
      if (idx >= 0)
        code = 48 + idx
      else
        idx = find(P3, ch)
        if (idx >= 0)
          code = 58 + idx
        else
          idx = find(P4, ch)
          if (idx >= 0)
            code = 65 + idx
          else
            idx = find(P5, ch)
            if (idx >= 0)
              code = 91 + idx
            else
              idx = find(P6, ch)
              if (idx >= 0)
                code = 97 + idx
              else
                idx = find(P7, ch)
                if (idx >= 0)
                  code = 123 + idx
                else
                  // non-printable -> 0
                  code = 0
    push(out, code)
    i = i + 1
  return out
