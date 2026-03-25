#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-03-25
 */

// Parse env-style args: ARGS="key=val foo=bar" (since argv is not available here)
// String helpers
#include <strings.fun>
line = env("ARGS")
if (len(line) == 0)
  line = "name=Fun color=blue"

parts = str_split(line, " ")
cfg = {}
for p in parts
  if (len(p) == 0) continue
  eq = find(p, "=")
  if (eq > 0)
    k = substr(p, 0, eq)
    v = substr(p, eq + 1, len(p) - eq - 1)
    cfg[k] = v

for k in keys(cfg)
  print(k + ": " + to_string(cfg[k]))

/* Expected output (order of keys may vary):
name: Fun
color: blue

With ARGS overridden, e.g.: ARGS="x=1 y=2"
x: 1
y: 2
*/
