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

print(bxor(0x80000000, 0x00000001))
print(bor(0x80000000, 0x00000001))
print(band(0x80000000, 0x80000000))
print(bnot(0x80000000))
print(bnot(0x7FFFFFFF))

/* Expected output:
2147483649
2147483649
2147483648
2147483647
2147483648
*/
