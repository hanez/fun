#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-12-10
 */

// Demonstrate byte overflow with try/catch
// Note: Runtime exceptions are not yet implemented; overflow emits an error and halts.
// This example shows intended usage once exceptions are supported.

print("=== byte overflow with try/catch demo ===")
try
  byte b = 0
  print("assign 255 -> ok")
  b = 255
  print(b)
  print("assign 256 -> should overflow and be caught")
  b = 256           // will trigger OverflowError: value out of range for uint8
  print("this line will not execute if overflow occurs")
catch err
  print("caught error:")
  print(err)
finally
  print("finally block executed")

/* Expected output:
=== byte overflow with try/catch demo ===
assign 255 -> ok
255
assign 256 -> should overflow and be caught
caught error:
OverflowError: value out of range for uint8
finally block executed
*/
