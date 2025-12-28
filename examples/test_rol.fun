#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-12-27
 */

print(rol(0x12345678, 4)) // Should be 0x23456781
print(rol(0x80000000, 1)) // Should be 0x00000001
print(rol(0x00000001, 31)) // Should be 0x80000000

/* Expected output:
591751041
1
2147483648
*/
