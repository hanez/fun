/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the ISC license.
 * https://opensource.org/license/isc-license-txt
 */

// Simple utility module to be included locally with: #include "include_local_util.fun"

fun greet(name)
  print("Hello, " + name)

fun sum(a, b)
  return a + b
