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

// Demo of C99-like floor(x)

print("=== math floor demo start ===")

print("floor(3.7) -> " + to_string(floor(3.7)))     // 3
print("floor(3.0) -> " + to_string(floor(3.0)))     // 3
print("floor(-3.2) -> " + to_string(floor(-3.2)))   // -4
print("floor(-3.0) -> " + to_string(floor(-3.0)))   // -3
print("floor(0.5) -> " + to_string(floor(0.5)))     // 0
print("floor(-0.5) -> " + to_string(floor(-0.5)))   // -1

print("=== math floor demo end ===")

/* Expected output (values):
=== math floor demo start ===
floor(3.7) -> 3
floor(3.0) -> 3
floor(-3.2) -> -4
floor(-3.0) -> -3
floor(0.5) -> 0
floor(-0.5) -> -1
=== math floor demo end ===
*/
