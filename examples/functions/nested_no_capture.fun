#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-04-02
 */

/*
 * Demonstrates nested functions that are only visible inside the
 * outer function where they are defined. This example avoids
 * capturing outer variables (closures) and instead passes values
 * explicitly, which works with the current implementation.
 */

print("=== Nested (no captures), explicit parameters ===")

fun outer(a, b)
  fun mul3(x)
    return x * 3

  fun combine(x, y)
    return x + y + 1

  ax = mul3(a)
  by = mul3(b)
  return combine(ax, by)

print("outer(2, 5) -> expected 2*3 + 5*3 + 1 = 22")
print(outer(2, 5))

/* Expected output:
=== Nested (no captures), explicit parameters ===
outer(2, 5) -> expected 2*3 + 5*3 + 1 = 22
22
*/
