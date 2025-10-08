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

// Demonstration of the include-as namespace feature.
//
// Run examples (without installing) by pointing FUN_LIB_DIR to the repo lib:
//   Linux/macOS/FreeBSD:
//     FUN_LIB_DIR="$(pwd)/lib" ./build/fun examples/include_namespace.fun
//   Windows (CMD):
//     set FUN_LIB_DIR=%CD%\lib && build-debug\fun.exe examples\include_namespace.fun
//   Windows (PowerShell):
//     $env:FUN_LIB_DIR="$PWD\lib"; .\build\fun.exe examples\include_namespace.fun

// Import stdlib math helpers into alias 'm'
#include <utils/math.fun> as m

// Import a local module (this repository file) into alias 'mod'
#include "examples/namespaced_mod.fun" as mod

print("=== include-as namespace demo ===")

print("Using m.add and m.times from <utils/math.fun>:")
print("m.add(2, 3)   = " + to_string(m.add(2, 3)))
print("m.times(4, 5) = " + to_string(m.times(4, 5)))

print("")
print("Using mod.hello and mod.Greeter from namespaced_mod.fun:")
print(mod.hello("Fun"))

g = mod.Greeter("Hi")
g.say("World")

print("")
print("=== done ===")

/* Expected output:
=== include-as namespace demo ===
Using m.add and m.times from <utils/math.fun>:
m.add(2, 3)   = 5
m.times(4, 5) = 20

Using mod.hello and mod.Greeter from namespaced_mod.fun:
Hello, Fun!
Hi World

=== done ===
*/
