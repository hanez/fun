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

// Example: Using stdlib PCRE2 class to run a simple test
// Requires building Fun with -DFUN_WITH_PCRE2=ON

#include <regex/pcre2.fun>

rx = PCRE2()

pattern = "^hello$"
text = "Hello"  // mismatches unless case-insensitive is set

// Use case-insensitive flag from the class helper
flags = rx.i()

print("pcre2_lib_test example:")
print(rx.test(pattern, text, flags))  // prints 1 on match, 0 otherwise

/* Expected output:
pcre2_lib_test example:
foo-123
4
11
2
*/
