#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-11-25
 */

/*
include <regex/pcre2.fun>

rx = Pcre2()

text = "E-mails: one@example.com, Two@Example.COM; invalid: x@y"
pattern = "[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}"

print("Has email? ", rx.test(pattern, text, rx.i()))

first = rx.match(pattern, text, rx.i())
if first != nil {
  print("First: ", first["full"]) 
}

all = rx.find_all(pattern, text, rx.i())
for m in all {
  print("Found: ", m["full"]) 
}
*/
