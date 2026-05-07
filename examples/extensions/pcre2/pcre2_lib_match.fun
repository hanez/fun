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

// Example: Using stdlib PCRE2 class to perform a single match
// Requires building Fun with -DFUN_WITH_PCRE2=ON

#include <regex/pcre2.fun>

rx = PCRE2()

pattern = "(foo)-(\\d+)"
text = "bar foo-123 baz"

// Default UTF flag is fine; show combining flags as well (UTF | I)
flags = bor(rx.u(), rx.i())

print("pcre2_lib_match example:")
m = rx.match(pattern, text, flags)
if (m != nil)
  print(m["full"])   // foo-123
  print(m["start"])  // start index
  print(m["end"])    // end index (exclusive)
  print(len(m["groups"])) // 2 groups captured
else
  print("no match")

/* Expected output:
pcre2_lib_match examples:
foo-123
4
11
2
*/
