#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-09-29
 */

include <crypt/sha512.fun>

sha = SHA512()

print("=== SHA-512 demo (hex input) ===")

// "" (empty)
print(sha.sha512_hex(""))  // -> cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce
                           //    47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e

// "abc"
print(sha.sha512_hex("616263"))  // -> ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a
                                 //    2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f

// "message digest"
print(sha.sha512_hex("6d65737361676520646967657374"))
// -> 107dbf389d9e9f71a3a95f6c055b9251
//    bc5268c2be16d6c13492ea45b0199f33
//    09e16455ab1e96118e8a905d5597b721
//    3f36cc64dab6c0c5

print("=== Done ===")

/* Expected output:
=== SHA-512 demo (hex input) ===
cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e
ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f
107dbf389d9e9f71a3a95f6c055b9251bc5268c2be16d6c13492ea45b0199f3309e16455ab1e96118e8a905d5597b72038ddb372a89826046de66687bb420e7c
=== Done ===
*/
