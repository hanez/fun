#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-03-25
 */

/*
 * Iterating through maps in Fun
 *
 * This example shows common patterns to iterate over a map:
 * - for k in keys(m): access value via m[k]
 * - for v in values(m): iterate values directly
 * - Notes about non-deterministic key order unless you define one
 */

// Define a sample map
user = { "name": "Ada", "age": 37, "city": "London" }

print("-- Iterate keys, then index map --")
for k in keys(user)
  print(k + ": " + to_string(user[k]))

print("-- Iterate values only --")
for v in values(user)
  print(v)

// If you need a specific order, provide it explicitly
order = ["name", "city", "age"]
print("-- Deterministic order by explicit key list --")
for k in order
  if has(user, k)
    print(k + ": " + to_string(user[k]))

/* Expected output (key order in the first two blocks may vary):
-- Iterate keys, then index map --
name: Ada
age: 37
city: London
-- Iterate values only --
Ada
37
London
-- Deterministic order by explicit key list --
name: Ada
city: London
age: 37
*/
