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

#include <strings.fun>

txt = "  one, two , three  "
parts = str_split(txt, ",")
i = 0
while (i < len(parts))
  parts[i] = str_trim(parts[i])
  i = i + 1
print("parts: " + to_string(parts))
joined = join(parts, ";")
print("joined: " + joined)

/* Expected output:
parts: [array n=3]
joined: one;two;three
*/
