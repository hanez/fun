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

// Example: Using stdlib PCRE2 class to find all matches
// Requires building Fun with -DFUN_WITH_PCRE2=ON

#include <regex/pcre2.fun>

rx = PCRE2()

pattern = "[A-Za-z]+"
text = "One two THREE four"

// Case-insensitive via class flag helper
flags = rx.i()

print("pcre2_lib_findall example:")
matches = rx.find_all(pattern, text, flags)
for m in matches
  print(m["full"]) // prints matched word

/* Expected output:
pcre2_lib_findall examples:
One
two
THREE
four
*/
