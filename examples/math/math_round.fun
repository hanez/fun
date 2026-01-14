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

// Demo of C99-like round(x) — half away from zero

print("=== math round demo start ===")

print("round(3.2) -> " + to_string(round(3.2)))     // 3
print("round(3.5) -> " + to_string(round(3.5)))     // 4
print("round(3.8) -> " + to_string(round(3.8)))     // 4
print("round(-3.2) -> " + to_string(round(-3.2)))   // -3
print("round(-3.5) -> " + to_string(round(-3.5)))   // -4
print("round(-3.8) -> " + to_string(round(-3.8)))   // -4

print("=== math round demo end ===")

/* Expected output (values):
=== math round demo start ===
round(3.2) -> 3
round(3.5) -> 4
round(3.8) -> 4
round(-3.2) -> -3
round(-3.5) -> -4
round(-3.8) -> -4
=== math round demo end ===
*/
