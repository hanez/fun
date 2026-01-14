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

// Demo of C99-like trunc(x) — rounds toward zero

print("=== math trunc demo start ===")

print("trunc(3.7) -> " + to_string(trunc(3.7)))     // 3
print("trunc(3.0) -> " + to_string(trunc(3.0)))     // 3
print("trunc(-3.7) -> " + to_string(trunc(-3.7)))   // -3
print("trunc(-3.0) -> " + to_string(trunc(-3.0)))   // -3
print("trunc(0.5) -> " + to_string(trunc(0.5)))     // 0
print("trunc(-0.5) -> " + to_string(trunc(-0.5)))   // 0

print("=== math trunc demo end ===")

/* Expected output (values):
=== math trunc demo start ===
trunc(3.7) -> 3
trunc(3.0) -> 3
trunc(-3.7) -> -3
trunc(-3.0) -> -3
trunc(0.5) -> 0
trunc(-0.5) -> 0
=== math trunc demo end ===
*/
