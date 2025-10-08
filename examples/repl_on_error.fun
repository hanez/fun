#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-06
 */

// Demonstration of --repl-on-error debugging.
// Run: fun --repl-on-error examples/repl_on_error.fun
// When the runtime error occurs, the REPL opens. Try commands:
//   :backtrace
//   :stack
//   :locals
//
// This script triggers an index-out-of-range error inside a nested call.

fun inner()
  a = [1, 2, 3]
  // Out-of-range access to cause a runtime error:
  print(a[10])

fun outer()
  inner()

outer()

/* Expected output (ruuning with --repl-on-error you will end up in the REPL 
 * with full stack access; :backtrace, :stack and :locals):
Runtime error: index out of range
 (at ./examples/repl_on_error.fun:12 in inner, op INDEX_GET @ip 9)
Entering REPL due to runtime error (code 1)
 (at ./examples/repl_on_error.fun:12 in inner, op INDEX_GET @ip 9)
Fun 0.25.0 REPL
Type :help for commands. Submit an empty line to run.
fun>
*/
