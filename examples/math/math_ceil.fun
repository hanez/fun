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

// Demo of C99-like ceil(x)

print("=== math ceil demo start ===")

print("ceil(3.2) -> " + to_string(ceil(3.2)))       // 4
print("ceil(3.0) -> " + to_string(ceil(3.0)))       // 3
print("ceil(-3.2) -> " + to_string(ceil(-3.2)))     // -3
print("ceil(-3.0) -> " + to_string(ceil(-3.0)))     // -3
print("ceil(0.5) -> " + to_string(ceil(0.5)))       // 1
print("ceil(-0.5) -> " + to_string(ceil(-0.5)))     // 0

print("=== math ceil demo end ===")

/* Expected output (values):
=== math ceil demo start ===
ceil(3.2) -> 4
ceil(3.0) -> 3
ceil(-3.2) -> -3
ceil(-3.0) -> -3
ceil(0.5) -> 1
ceil(-0.5) -> 0
=== math ceil demo end ===
*/
