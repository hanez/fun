/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-01
 */

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
