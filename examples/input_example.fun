#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-02
 */

/*
 * Interactive input example using input() and Console wrapper.
 */

#include <io/console.fun>

print("=== input() builtin ===")
name = input("Enter your name: ")
print(join(["Hello, ", name, "!"], ""))

print("=== Console class ===")
c = Console()
city = c.ask("Your city")
print(join(["Nice to meet you from ", city], ""))

yn = c.ask_yes_no("Do you like Fun?")
if yn
  print("Great! 🎉")
else
  print("Give it a try, it grows on you.")

/* Input given:
Enter your name: Hanez
Your city: Universe
Do you like Fun? [y/n]: y
*/

/* Expected output:
=== input() builtin ===
Hello, Hanez!
=== Console class ===
Nice to meet you from Universe
Great! 🎉
*/
