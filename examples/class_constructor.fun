#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-01
 */

/*
 * Demonstrates class constructors named _construct(this, ...)
 * The constructor is called automatically after instance creation,
 * receiving 'this' and all header parameters as arguments.
 */

print("=== class constructor demo ===")

class Counter(number start)
  // default field value (will be overwritten by _construct)
  value = 0

  // Runs automatically with (this, start)
  fun _construct(this, s)
    this.value = s

  fun inc(this)
    this.value = this.value + 1
    return this.value

c = Counter(10)
print("initial value=" + to_string(c.value))  // expect 10
print("after inc=" + to_string(c.inc()))      // expect 11

print("=== done ===")

/* Expected output:
=== class constructor demo === 
initial value=10
after inc=11
=== done ===
*/
