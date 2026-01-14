#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-01-13
 */

/*
 * Base64 usage demo (RFC 4648, standard alphabet)
 *
 * Run without installing:
 *   FUN_LIB_DIR="$(pwd)/lib" ./build/fun examples/base64_demo.fun
 */

#include <encoding/base64.fun>

print("=== Base64 demo ===")

// Bytes for the ASCII string "Hello"
bytes = [0x48, 0x65, 0x6c, 0x6c, 0x6f]

// Encode bytes -> Base64 string
b64 = b64_encode_bytes(bytes)
print("b64(Hello) = " + b64)            // Expected: SGVsbG8=

// Decode Base64 -> bytes array (uncomment to try)
// decoded = b64_decode_to_bytes(b64)
// // Build a comma-separated list of byte values as strings
// parts = []
// for i in range(0, len(decoded))
//   push(parts, to_string(decoded[i]))
// print("bytes = " + join(parts, ","))  // Expected: 72,101,108,108,111

// Another example: padding with '='
bytes2 = [0x46, 0x75, 0x6e]  // "Fun"
print("b64(Fun) = " + b64_encode_bytes(bytes2))  // Expected: RnVu

print("=== done ===")

/* Expected output:
=== Base64 demo ===
b64(Hello) = SGVsbG8=
b64(Fun) = RnVu
=== done ===
*/
