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

path = "./examples/data/sample.csv"

// Very small CSV reader (no quotes/escapes) for demo purposes
fun parse_csv_line(line)
  return str_split(str_trim(line), ",")

data = read_file(path)
if (len(data) == 0)
  print("No CSV content at: " + path)
else
  rows = str_split(data, "\n")
  for row in rows
    if (len(str_trim(row)) == 0) continue
    cols = parse_csv_line(row)
    print("ROW: " + to_string(cols))

/* Expected output (from examples/io/sample.csv):
ROW: [array n=3]
ROW: [array n=3]
ROW: [array n=3]
ROW: [array n=3]
*/
