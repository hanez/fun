/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-03-30
 */

// Minimal test library to trigger a runtime error at a precise line

fun boom()
  // The division by zero MUST be on this exact line (line 14)
  x = 1 / 0
  return x

fun ok()
  return 42
