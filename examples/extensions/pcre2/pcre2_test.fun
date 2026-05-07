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

// Minimal example showing only the pcre2_test opcode
// Requires building Fun with -DFUN_WITH_PCRE2=ON

pattern = "^hello$"
text = "Hello"  // mismatches unless case-insensitive is set

// Flags: 1=I (ignore case), 2=M, 4=S, 8=U (UTF), 16=X
flags = 1  // make it case-insensitive so it matches

print("pcre2_test example:")
print(pcre2_test(pattern, text, flags))  // prints 1 on match, 0 otherwise

/* Expected output:
pcre2_test example:
1
*/
