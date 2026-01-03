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

// Demo of fmin(x,y) / fmax(x,y) — float-aware min/max with C99 NaN handling

print("=== math fmin/fmax demo start ===")

print("fmin(3.2, 4) -> " + to_string(fmin(3.2, 4)))   // 3.2
print("fmax(3.2, 4) -> " + to_string(fmax(3.2, 4)))   // 4
print("fmin(-5, -2.5) -> " + to_string(fmin(-5, -2.5))) // -5
print("fmax(-5, -2.5) -> " + to_string(fmax(-5, -2.5))) // -2.5

print("=== math fmin/fmax demo end ===")

/* Expected output:
=== math fmin/fmax demo start ===
fmin(3.2, 4) -> 3.2000000000000002
fmax(3.2, 4) -> 4
fmin(-5, -2.5) -> -5
fmax(-5, -2.5) -> -2.5
=== math fmin/fmax demo end ===
*/
