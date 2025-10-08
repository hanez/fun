#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-04
 */

// Procedural regex demo using built-in functions.
// Shows internal power without the Regex class wrapper.

text = "abc-123-xyz"

// Full string match
m1 = regex_match(text, "[a-z]+-[0-9]+-[a-z]+$")
print(m1)

// First search with groups (prints a map)
s1 = regex_search(text, "([0-9])([0-9])([0-9])")
print(s1)

// Global replace: replace digits with '#'
out = regex_replace(text, "[0-9]", "#")
print(out)

/* Expected output:
1
{"match": 123, "start": 4, "end": 7, "groups": [1, 2, 3]}
abc-###-xyz
*/
