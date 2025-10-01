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

// Include the pure Fun SHA-256 library
include <crypt/sha256.fun>

sha = SHA256()

print("=== SHA-256 demo (hex input) ===")

// "" (empty)
print(sha.sha256_hex(""))  // -> e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855

// "abc"
print(sha.sha256_hex("616263"))  // -> ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad

// "message digest"
print(sha.sha256_hex("6d65737361676520646967657374")) // -> f7846f55cf23e14eebeab5b4e1550cad5b509e3348fbc4efa3a1413d393cb650

// "abcdefghijklmnopqrstuvwxyz"
print(sha.sha256_hex("6162636465666768696a6b6c6d6e6f707172737475767778797a")) // -> 71c480df93d6ae2f1efad1447c66c9525e316218cf51fc8d9ed832f2daf18b73

print("=== Done ===")

/* Expected output:
=== SHA-256 demo (hex input) ===
e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
3859a4ec00dcbed7c01d4a462d18c1f4097a690a1d4d459be4e5b6f5a594ee68
de8ff7370090823795e96e3b7fbee3cfad374492417da99d72ba55297f7e58ab
5d3ed26c5e75585f56e664c0ebc63aa185ec606109f6374fefa42c862670d257
=== Done ===
*/
