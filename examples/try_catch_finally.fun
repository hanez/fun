/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-03
 */

// Demonstration of try/catch/finally syntax support
// Note: current runtime does not yet implement exceptions; catch blocks are not executed.

print("before try")
try
  print("inside try body")
catch err
  // This will not run yet (no exception raised, and runtime doesn't support throwing)
  print("caught error: ")
  print(err)
finally
  print("finally always runs (syntactically)")
print("after try")
