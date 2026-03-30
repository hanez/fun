#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-03-30
 */

/*
 Test: error location in included files shows correct file:line

 How to run:
   - From repo root: ./build_debug/fun examples/error/include_line_mapping_test.fun

 Expected: Runtime error message suffix contains the included file path
           (lib/test/include_divzero.fun) and the exact line number of
           the failing operation inside that file (see lib file below).
*/

// Use angle-bracket include so resolution tries FUN_LIB_DIR, DEFAULT_LIB_DIR,
// and finally the project-local lib/ directory. This makes the example work
// from the repository without extra setup.
#include <test/include_divzero.fun>

fun main()
  // Trigger the division by zero inside the included file
  boom()

main()
