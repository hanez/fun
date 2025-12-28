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

print(bxor(0xefcdab89, 0x98badcfe))
print(bxor(0x77767677, 0x10325476))
print(bxor(bxor(0xefcdab89, 0x98badcfe), 0x10325476))

/* Expected output:
2004318071
1732518401
1732584193
*/
