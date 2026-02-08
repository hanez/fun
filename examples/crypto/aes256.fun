#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-02-04
 */

/*
 * Example: Using AES-256 (ECB) from lib/crypt/aes256.fun
 *
 * Test vector (FIPS-197, AES-256):
 *   key: 000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f
 *   pt : 00112233445566778899aabbccddeeff
 *   ct : 8ea2b7ca516745bfeafc49904b496089
 */

#include <crypt/aes256.fun>

aes = AES256()

key = "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f"
pt  = "00112233445566778899aabbccddeeff"

ct  = aes.encrypt_block_hex(pt, key)
print(ct)

// Multi-block ECB example (two same blocks => two same ciphertext blocks)
//pt2 = join([pt, pt], "")
//ct2 = aes.encrypt_ecb_hex(pt2, key)
//print(ct2)

/* Expected output:
8ea2b7ca516745bfeafc49904b496089
*/

/* Wanted doutput:
8ea2b7ca516745bfeafc49904b496089
8ea2b7ca516745bfeafc49904b4960898ea2b7ca516745bfeafc49904b496089
*/
