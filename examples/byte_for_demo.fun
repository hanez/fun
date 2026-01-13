#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

// Byte + for-loop demonstration

print("=== byte with hex literal and clamping ===")

byte b = 0x1223       // 0x1223 -> 0x23 after 8-bit clamp
print(b)              // -> 35
b = 0xFF
print(b)              // -> 255
b = 0x1FF            // 0x1FF -> 0xFF after clamp
print(b)              // -> 255

print("")
print("=== for-loop reassigning a byte variable (shows clamping near the top end) ===")

byte acc = 0
for i in range(250, 260)
  acc = i           // will clamp at 255
  print(acc)

print("")
print("=== dynamic vs typed ===")

x = 0xAB
print(typeof(x))     // -> "Number"
x = "now string"     // dynamic variable can change type
print(typeof(x))     // -> "String"

// Typed byte must remain numeric (uint8):
// b = "oops"        // Uncomment to see a runtime type error

/* Expected output:
=== byte with hex literal and clamping ===
OverflowError: value out of range for uint8
*/

/* Expected output (OLD):
=== byte with hex literal and clamping ===
255
255
255

=== for-loop reassigning a byte variable (shows clamping near the top end) ===
250
251
252
253
254
255
255
255
255
255

=== dynamic vs typed ===
Number
String
*/
