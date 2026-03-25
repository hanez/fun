#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-03-25
 */

#include <strings.fun>

path = "./tmp/example.txt"
print("Writing to: " + path)
write_file(path, "Hello from Fun!\n")
txt = read_file(path)
print("Read back: " + str_trim(txt))

/* Expected output:
Writing to: ./tmp/example.txt
Read back: Hello from Fun!
*/
