#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-12-12
 */

/*
 * Example: Using OS-based random_number(len) builtin
 * This generates cryptographically strong random bytes and returns them
 * hex-encoded as a string of length 2*len (since each byte -> two hex chars).
 */

#include <hex.fun>

print("--- random_number(len) demo ---")

len_bytes = 16
hexstr = random_number(len_bytes)
print("Requested bytes: " + to_string(len_bytes))
print("Hex string: " + hexstr)
print("Hex bytes array: " + to_string(hex_to_bytes(hexstr)))
echo("Hex dump to bytes: ")
print(hex_to_bytes(hexstr))
print("Hex length (should be 2*bytes = 32): " + to_string(len(hexstr)))

// Zero length returns empty string
empty = random_number(0)
print("Empty (0 bytes) -> length: " + to_string(len(empty)) + " value: " + to_string(empty))

// You can request longer values as needed, e.g., 32 bytes -> 64 hex chars
hex64 = random_number(32)
print("32 bytes -> " + to_string(len(hex64)) + " hex chars")

/* Possible output:
--- random_number(len) demo ---
Requested bytes: 16
Hex string: 8497373c7fb52c6cb7f1e1fda5bb6a60
Hex bytes array: [array n=16]
Hex dump to bytes: [132, 151, 55, 60, 127, 181, 44, 108, 183, 241, 225, 253, 165, 187, 106, 96]
Hex length (should be 2*bytes = 32): 32
Empty (0 bytes) -> length: 0 value: 
32 bytes -> 64 hex chars
*/
