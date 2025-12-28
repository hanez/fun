#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-12-28
 */

class Other(number val)
  fun _construct(this, val)
    this.v = val

class Test()
  fun _construct(this)
    val = Other(123)
    this.a = val
    print("a set")

t = Test()
print(t.a.v)

/* Expected output:
a set
123
*/
