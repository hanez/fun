#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-02-19
 */

/*
 * Demonstrates dynamic match() helper.
 */

#include <utils/option.fun>

print("-- match-like with numbers --")
x = 0
if (x == -1)
  print("minus one")
else if (x == 0)
  print("zero")
else if (x > 0)
  print("positive: " + to_string(x))
else
  print("negative: " + to_string(x))

print("-- match-like with Option --")
o1 = some(42)
o2 = none()

if is_some(o1)
  print("Some(" + to_string(unwrap(o1)) + ")")
else
  print("None")

if is_some(o2)
  print("Some(" + to_string(unwrap(o2)) + ")")
else
  print("None")
