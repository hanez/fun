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

// Demo: SHA-256 of raw strings (no hex decoding)
include "lib/crypt/sha256.fun"

sha = SHA256()

print("=== SHA-256 demo (raw string input via sha256_str) ===")

// "" (empty string)
print(sha.sha256_str(""))  // -> e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855

// "abc"
print(sha.sha256_str("abc"))  // -> 3859a4ec00dcbed7c01d4a462d18c1f4097a690a1d4d459be4e5b6f5a594ee68

// "616263" (the literal characters '6','1','6','2','6','3')
print(sha.sha256_str("616263"))  // -> faee5966a2c104cf2287085428a183fe58c18afdc5ff261571e8e9c4796ad635

// "message digest"
print(sha.sha256_str("6d65737361676520646967657374")) // -> 

// "abcdefghijklmnopqrstuvwxyz"
print(sha.sha256_str("6162636465666768696a6b6c6d6e6f707172737475767778797a")) // -> 

print("=== Note ===")
print("sha256_str(\"616263\") hashes the text 616263; sha256_hex(\"616263\") hashes bytes 0x61 0x62 0x63 (abc).")
