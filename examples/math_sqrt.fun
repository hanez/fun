#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 *  Added: 2026-01-03
 */

// Demo of sqrt(x)

print("=== math sqrt demo start ===")

print("sqrt(0) -> " + to_string(sqrt(0)))
print("sqrt(9) -> " + to_string(sqrt(9)))
print("sqrt(2) -> " + to_string(sqrt(2)))
print("sqrt(-1) -> " + to_string(sqrt(-1)))  // NaN expected

print("=== math sqrt demo end ===")

/* Expected output (approximate):
=== math sqrt demo start ===
sqrt(0) -> 0
sqrt(9) -> 3
sqrt(2) -> 1.41421356...
sqrt(-1) -> nan
=== math sqrt demo end ===
*/
