#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-12-18
 */

#include <hex.fun>

print("Testing hex_to_dec:")
print("0x0 -> " + to_string(hex_to_dec("0")))
print("0xf -> " + to_string(hex_to_dec("f")))
print("0x10 -> " + to_string(hex_to_dec("10")))
print("0xff -> " + to_string(hex_to_dec("ff")))
print("0x100 -> " + to_string(hex_to_dec("100")))
print("0xdeadbeef -> " + to_string(hex_to_dec("deadbeef")))

/* Expected output:
Testing hex_to_dec:
0x0 -> 0
0xf -> 15
0x10 -> 16
0xff -> 255
0x100 -> 256
0xdeadbeef -> 3735928559
*/
