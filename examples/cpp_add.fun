#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-01-29
 */

/*
Build:
  cmake -S . -B build_debug -DFUN_WITH_CPP=ON
  cmake --build build_debug --target fun
Run:
  build_debug/fun examples/cpp_add.fun
*/

number x = cpp_add(2, 40)
//print("typeof(x): " + typeof(x))
print(x)

/* Expected output:
42
*/
