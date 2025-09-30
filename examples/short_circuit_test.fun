#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */
 
// Short-circuit demo for || and &&

print("=== short-circuit demo ===")

fun mark(name, v)
  // Show when this branch is evaluated
  print(name)
  return v

// OR short-circuits: RHS not evaluated when LHS is true
if (true || mark("OR-RHS", 0))
  print(1)          // expect: prints 1; no "OR-RHS"
else
  print(0)

// AND short-circuits: RHS not evaluated when LHS is false
if (false && mark("AND-RHS", 1))
  print(1)
else
  print(0)          // expect: prints 0; no "AND-RHS"

// OR evaluates RHS when needed (LHS false)
if (false || mark("OR-NEEDED", 1))
  print(1)          // expect: prints "OR-NEEDED" then 1
else
  print(0)

// AND evaluates RHS when needed (LHS true)
if (true && mark("AND-NEEDED", 1))
  print(1)          // expect: prints "AND-NEEDED" then 1
else
  print(0)

print("=== end ===")
