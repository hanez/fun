#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

// Have fun in Fun

string s = "Have fun!"

fun have_fun()
  print("Have fun!")
  print(s)
  number n = 10
  print(n)

have_fun()

print("Have fun!")

/* Expected output:
Have fun!
Have fun!
10
Have fun!
*/

