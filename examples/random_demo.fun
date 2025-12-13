#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-12-11
 */

// Demonstrates usage of RANDOM_SEED and RANDOM_INT opcodes via
// the built-ins: random_seed(seed) and random_int(lo, hiExclusive).

print("-- Random demo: seed reproducibility and bounds --")

// Seed with a fixed value and produce a short sequence
seed = 123456
random_seed(seed)
a1 = random_int(0, 10)   // in [0,10)
a2 = random_int(0, 10)
a3 = random_int(5, 8)    // in [5,8)

print("First run:")
print(a1)
print(a2)
print(a3)

// Re-seed with the same value: the sequence should repeat
random_seed(seed)
b1 = random_int(0, 10)
b2 = random_int(0, 10)
b3 = random_int(5, 8)

print("Second run (after re-seed):")
print(b1)
print(b2)
print(b3)

print("Reproducible? (a1==b1, a2==b2, a3==b3)")
print(a1 == b1)
print(a2 == b2)
print(a3 == b3)

// Show that the upper bound is exclusive by sampling multiple times
// and tracking the maximum seen value; it should never reach hi.
lo = 10
hi = 20
max_seen = lo
i = 0
while i < 100
  v = random_int(lo, hi)
  if (v > max_seen) max_seen = v
  i = i + 1

print("Max seen in [" + to_string(lo) + "," + to_string(hi) + ") over 100 samples:")
print(max_seen)
print("Is max_seen < hi? ")
print(max_seen < hi)

/* Expected output:
-- Random demo: seed reproducibility and bounds --
First run:
9
3
5
Second run (after re-seed):
9
3
5
Reproducible? (a1==b1, a2==b2, a3==b3)
true
true
true
Max seen in [10,20) over 100 samples:
19
Is max_seen < hi? 
1
*/
