#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the ISC license.
 * https://opensource.org/license/isc-license-txt
 */

print("=== Line-by-line print start ===")

string file = "/etc/passwd"

// Print each line in a file
fun print_file_lines(path)
  content = read_file(path)
  lines = split(content, "\n")
  for line in lines
    print(line)

print("Printing file: " + file)
print_file_lines(file)

print("=== Line-by-line print end ===")

/* Expected output:
? The content of /etc/passwd
Like: cat /etc/passwd
*/
