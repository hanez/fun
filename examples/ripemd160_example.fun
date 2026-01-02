#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-01-02
 */

// THIS IS BROKEN ACTUALLY! IT DOES NOT CALCULATE THE EXPECTED HASHES!

/*
 * Example: Using RIPEMD160 from lib/crypt/ripemd160.fun
 *
 * Known vectors:
 *  - "" (empty) => 9c1185a5c5e9fc54612808977ee8f548b2258d31
 *  - "abc"      => 8eb208f7e05d987a9b04... (full known); we verify via hex of "abc"
 *  - "The quick brown fox jumps over the lazy dog" => 37f332f68db77bd9d7edd4969571ad671cf9dd3b
 */

#include <crypt/ripemd160.fun>

r = RIPEMD160()

// empty string
print(r.ripemd160_str(""))

// "abc"
print(r.ripemd160_hex("616263"))

// pangram
print(r.ripemd160_str("The quick brown fox jumps over the lazy dog"))
