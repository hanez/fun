#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/*
 * Line-by-Line File Processing
 *
 * Shows techniques for:
 * - Reading files line by line using `readline`
 * - Conditional processing based on line content
 * - Maintaining state between lines (e.g., counters)
 * - Implementing custom file formatters
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
