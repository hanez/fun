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

// THIS IS BROKEN ACTUALLY! IT DOES NOT CALCULATE THE EXPECTED HASHES!

// Example usage of the RIPEMD-160 hash algorithm.

#include <crypt/ripemd160.fun>

rmd = RIPEMD160()

// Test case 1: Empty string
s1 = ""
h1 = rmd.ripemd160_str(s1)
print("RIPEMD-160(\"" + s1 + "\") = " + h1)
// Expected: 9c1185a5c5e9fc54612808977ee8f548b2258d31

// Test case 2: "abc"
s2 = "abc"
h2 = rmd.ripemd160_str(s2)
print("RIPEMD-160(\"" + s2 + "\") = " + h2)
// Expected: 8eb208f7e05d987a9b044a8e98c6b087f15a0bfc

// Test case 3: "message digest"
s3 = "message digest"
h3 = rmd.ripemd160_str(s3)
print("RIPEMD-160(\"" + s3 + "\") = " + h3)
// Expected: 5d0689ef49d2fae572b881b123a85ffa21595f36

// Example of hashing from hex string
hex_input = "616263" // "abc" in hex
h4 = rmd.ripemd160_hex(hex_input)
print("RIPEMD-160(hex: " + hex_input + ") = " + h4)
// Expected: 8eb208f7e05d987a9b044a8e98c6b087f15a0bfc

/* Expected output:
RIPEMD-160("") = 9c1185a5c5e9fc54612808977ee8f548b2258d31
RIPEMD-160("abc") = 8eb208f7e05d987a9b044a8e98c6b087f15a0bfc
RIPEMD-160("message digest") = 5d0689ef49d2fae572b881b123a85ffa21595f36
RIPEMD-160(hex: 616263) = 8eb208f7e05d987a9b044a8e98c6b087f15a0bfc
*/

/* Actual output:
RIPEMD-160("") = 560e66de427fc2c3399e86c3e1d38c3c6017def2
RIPEMD-160("abc") = 74266d6b4447493abb790c49e4bfa8f07263bcff
RIPEMD-160("message digest") = 80ee01c69badb632393a9e05d1e7eebc2f05ac48
RIPEMD-160(hex: 616263) = 74266d6b4447493abb790c49e4bfa8f07263bcff
*/
