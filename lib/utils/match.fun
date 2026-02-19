/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-02-19
 */

// A simple dynamic match helper.
// Usage:
// match(x, [
//   {"when": fun(v) v > 0, "then": fun(v) print("pos")},
//   {"is": 0, "then": fun(v) print("zero")},
//   {"else": fun(v) print("neg or other")}
// ])

fun match(value, cases)
  // Iterate cases in order; support keys: is, when, else
  for c in cases
    if (has_key(c, "is"))
      if (value == c["is"]) 
        th = c["then"]
        return th(value)
    else if (has_key(c, "when"))
      pred = c["when"]
      if (pred(value))
        th = c["then"]
        return th(value)
    else if (has_key(c, "else"))
      el = c["else"]
      return el(value)
  // No match; return nil
  return nil
