#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-04
 */

// Simple demo of Regex stdlib class.

include <regex.fun>

r = Regex()

text = "abc-123-xyz"

// full match (should be 0)
m1 = r.match(text, "[a-z]+-[0-9]+-[a-z]+$")
print(m1)

// search
s1 = r.search(text, "[0-9]+")
print(s1)

// replace digits with #
out = r.replace(text, "[0-9]", "#")
print(out)

/* Expected output:
1
{"match": 123, "start": 4, "end": 7, "groups": []}
abc-###-xyz
*/
