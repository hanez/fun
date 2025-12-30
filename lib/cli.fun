/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-12-30
 */

// Simple CLI helpers for Fun scripts.
//
// argv(): returns an array of the raw arguments passed to the script
//         (not including the script path). Requires the interpreter to
//         export FUN_ARGC and FUN_ARGV_i environment variables.
// parse_args(args): parses flags and positionals from argv() like:
//   -f  -> flags["f"] = 1
//   --force -> flags["force"] = 1
//   --force=42 -> flags["force"] = "42"
//   -abc -> flags["a"]=1, flags["b"]=1, flags["c"]=1

fun argv()
  n = to_number(env("FUN_ARGC"))
  a = []
  i = 0
  while (i < n)
    // Build key without join() to avoid any potential call resolution issues
    key = "FUN_ARGV_" + to_string(i)
    push(a, env(key))
    i = i + 1
  return a

// Returns: { "flags": map, "positionals": array }
fun parse_args(args)
  flags = {}
  pos = []
  i = 0
  n = len(args)
  while (i < n)
    s = args[i]
    slen = len(s)
    if (slen >= 2 && substr(s, 0, 2) == "--")
      eq = find(s, "=")
      if (eq >= 0)
        key = substr(s, 2, eq - 2)           // between -- and =
        val = substr(s, eq + 1, slen - (eq + 1))
        flags[key] = val
      else
        key = substr(s, 2, slen - 2)
        flags[key] = 1
    else if (slen == 2 && substr(s, 0, 1) == "-")
      // short -f boolean
      key = substr(s, 1, 1)
      flags[key] = 1
    else if (slen > 2 && substr(s, 0, 1) == "-")
      // compact short flags like -abc -> a=1, b=1, c=1
      j = 1
      while (j < slen)
        k = substr(s, j, 1)
        flags[k] = 1
        j = j + 1
    else
      push(pos, s)
    i = i + 1
  return { "flags": flags, "positionals": pos }
