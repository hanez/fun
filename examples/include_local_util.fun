/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

// Simple utility module to be included locally with: #include "include_local_util.fun"

fun greet(name)
  print("Hello, " + name)

fun sum(a, b)
  return a + b

/* Expected output:
None, it's a lib...
*/

