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

// Demo of cos(x)

print("=== math cos demo start ===")

print("cos(0) -> " + to_string(cos(0)))
print("cos(3.14159265359) ~ pi -> " + to_string(cos(3.14159265359)))
print("cos(1.57079632679) ~ pi/2 -> " + to_string(cos(1.57079632679)))

print("=== math cos demo end ===")

/* Expected output (approximate):
=== math cos demo start ===
cos(0) -> 1
cos(3.14159265359) ~ pi -> -1
cos(1.57079632679) ~ pi/2 -> 0
=== math cos demo end ===
*/
