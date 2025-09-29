#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the ISC license.
 * https://opensource.org/license/isc-license-txt
 *
 * Added: 2025-09-29
 */

// Include the pure Fun SHA-256 library
include "lib/crypt/sha256.fun"

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
