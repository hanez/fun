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

// Example: try/catch/finally with an intentional runtime error
// Note: The current runtime does not implement exceptions yet, so the catch
// block will not execute today. This example is prepared for when exceptions
// are supported. It also demonstrates the location of the error inside try.

print("before try")
try
  print("inside try: about to divide by zero")
  number x = 1 / 0  // deliberate runtime error
  print("this will not be reached due to the error above")
catch err
  // Intended behavior (once exceptions are wired up): this block runs
  // and 'err' contains an error object/stack trace
  print("caught error: ")
  print(err)
finally
  print("finally runs regardless (syntactically, even without exceptions)")
print("after try")
