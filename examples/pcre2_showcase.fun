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

re = PCRE2()

print("Testing PCRE2 showcase...")

print(re.test("\\d+", "Order #1234"))

m = re.match("(\\w+)", "hello WORLD", re.i())
if m != nil {
  print(m["full"])      // hello
  print(len(m["groups"]))
}

for x in re.find_all("[a-z]+", "One two THREE four", re.i()) {
  print(x["full"]) // one two four
}
*/
