#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-03-25
 */

// De-duplicate an array using a map as a set
arr = [1,2,2,3,1,4]
seen = {}
out = []
for x in arr
  if (!has(seen, to_string(x)))
    seen[to_string(x)] = 1
    push(out, x)
print(to_string(out))

/* Expected output:
[array n=4]
*/
