#!/usr/bin/env fun

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
