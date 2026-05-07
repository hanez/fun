#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-05-07
 */

// Minimal example showing only the pcre2_match opcode
// Requires building Fun with -DFUN_WITH_PCRE2=ON

pattern = "(foo)-(\\d+)"
text = "bar foo-123 baz"

// 8 = UTF flag is harmless here; kept for consistency
flags = 8

print("pcre2_match example:")
m = pcre2_match(pattern, text, flags)
if (m != nil)
  print(m["full"])   // foo-123
  print(m["start"])  // start index
  print(m["end"])    // end index (exclusive)
  print(len(m["groups"])) // 2 groups captured: foo and 123
else
  print("no match")

/* Expected output:
pcre2_match example:
foo-123
4
11
2
*/
