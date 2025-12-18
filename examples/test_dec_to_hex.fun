#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-12-18
 */

#include <hex.fun>

hexstr = "ff"
print("Hex to dec: " + to_string(hex_to_dec(hexstr)))
print("Dec to hex: " + dec_to_hex(hex_to_dec(hexstr)))

/* Expected output:
Hex to dec: 255
Dec to hex: ff
*/
