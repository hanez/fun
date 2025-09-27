#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the ISC license.
 * https://opensource.org/license/isc-license-txt
 */

// Environment variables via env(name):
// - Returns the value as a string.
// - If the variable is not set, returns an empty string.

print("HOME=" + env("HOME"))
print("SHELL=" + env("SHELL"))
print("FUN_NOT_SET=" + env("FUN_NOT_SET"))

// You can use it in scripts, e.g.:
fun greet()
  user = env("USER")
  if user == ""
    print("Hello, mysterious friend!")
  else
    print("Hello, " + user + "!")
greet()
