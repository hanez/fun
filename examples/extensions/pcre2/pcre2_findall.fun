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

// Minimal example showing only the pcre2_findall opcode
// Requires building Fun with -DFUN_WITH_PCRE2=ON

pattern = "[A-Za-z]+"
text = "One two THREE four"

// 1 = I (ignore case) so we catch mixed case words uniformly
flags = 1

print("pcre2_findall example:")
matches = pcre2_findall(pattern, text, flags)
for m in matches
  print(m["full"]) // prints matched word

/* Expected output:
pcre2_findall example:
One
two
THREE
four
*/
