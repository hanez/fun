#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-09-30
 */

// Threading demo for Fun
// Run without installing:
//   FUN_LIB_DIR="$(pwd)/lib" ./build/fun examples/threads_demo.fun
//   (Windows PowerShell) $env:FUN_LIB_DIR="$PWD\lib"; .\build\fun.exe examples\threads_demo.fun

print("=== Threads demo ===")

// A worker function that does some work and returns a value
fun square(n)
  // simulate work
  sleep(100)
  return n * n

// Spawn several workers
ids = []
for x in [1, 2, 3, 4, 5]
  id = thread_spawn(square, x)   // pass a single argument
  push(ids, id)
  print("spawned thread id=" + to_string(id) + " for x=" + to_string(x))

print("Joining threads...")
results = []
for id in ids
  r = thread_join(id)
  push(results, r)

print("Squares: " + to_string(results))

// Example: pass multiple args via array
fun add3(a, b, c)
  return a + b + c

tid = thread_spawn(add3, [10, 20, 30])
print("join add3 -> " + to_string(thread_join(tid)))

print("=== Threads demo done ===")

/* Expected output:
=== Threads demo ===
spawned thread id=1 for x=1
spawned thread id=2 for x=2
spawned thread id=3 for x=3
spawned thread id=4 for x=4
spawned thread id=5 for x=5
Joining threads...
Squares: [array n=5]
join add3 -> 60
=== Threads demo done ===
*/
