#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-09-29
 */

// Demo: SHA-512 of raw strings (no hex decoding)
include "lib/crypt/sha512.fun"

sha = SHA512()

print("=== SHA-512 demo (raw string input via sha512_str) ===")

// "" (empty string)
print(sha.sha512_str(""))  // -> cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce
                           //    47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e

// "abc"
print(sha.sha512_str("abc"))  // -> ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a
                              //    2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f

// "616263" (the literal characters '6','1','6','2','6','3')
print(sha.sha512_str("616263"))  // -> ee21095236a9c037f705f41ffa3cf60869891fcdb461c5c8fe50e5b1711a27b
                                 //    fc02de2e387228651bdd034113cc59af777fdb9c915b70fdbed0eeacf7113296b

print("=== Note ===")
print("sha512_str(\"616263\") hashes the text 616263; sha512_hex(\"616263\") hashes bytes 0x61 0x62 0x63 (abc).")
