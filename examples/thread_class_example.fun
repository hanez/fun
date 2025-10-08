#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-04
 */

/*
 * Thread class example using the stdlib Thread wrapper around thread_spawn/join.
 */

#include <io/thread.fun>

print("=== Thread class example ===")

// worker that returns a square
fun square(n)
  sleep(50)
  return n * n

// Another worker accepting 3 args
fun add3(a, b, c)
  return a + b + c

// Use the Thread class
thr = Thread()

ids = []
for x in [1, 2, 3, 4, 5]
  id = thr.spawn(square, x)
  push(ids, id)
  print("spawned thread id=" + to_string(id) + " for x=" + to_string(x))

print("Joining threads...")
results = []
for id in ids
  r = thr.join(id)
  push(results, r)

print("Squares: " + to_string(results))

// Pass multiple args via array
id2 = thr.start(add3, [10, 20, 30])
print("join add3 -> " + to_string(thr.wait(id2)))

print("=== Thread class example done ===")

/* Expected output (ids may vary):
=== Thread class example ===
spawned thread id=1 for x=1
spawned thread id=2 for x=2
spawned thread id=3 for x=3
spawned thread id=4 for x=4
spawned thread id=5 for x=5
Joining threads...
Squares: [array n=5]
join add3 -> 60
=== Thread class example done ===
*/
