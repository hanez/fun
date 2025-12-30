#!/usr/bin/env fun

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

// Demonstrates parsing flags like -f, --force, --force=42 and positionals
// Usage examples:
//   FUN_LIB_DIR="$(pwd)/lib" ./build_debug/fun ./examples/cli_parse_example.fun -f foo
//   FUN_LIB_DIR="$(pwd)/lib" ./build_debug/fun ./examples/cli_parse_example.fun --force bar
//   FUN_LIB_DIR="$(pwd)/lib" ./build_debug/fun ./examples/cli_parse_example.fun --force=42 alpha beta
//   FUN_LIB_DIR="$(pwd)/lib" ./build_debug/fun ./examples/cli_parse_example.fun -abc x y

#include <cli.fun>

args = argv()
parsed = parse_args(args)
flags = parsed["flags"]
pos = parsed["positionals"]

if (flags["f"] == 1 || flags["force"] == 1)
  print("Force enabled")

// No hasKey() builtin available here; check via keys()+contains()
flag_keys = keys(flags)
if (contains(flag_keys, "force") && typeof(flags["force"]) == "string")
  print(join(["Force value=", flags["force"]], ""))

print("Flags map:")
print(flags)
print("Positionals:")
print(pos)

/* Expected output (FUN_LIB_DIR="$(pwd)/lib" ./build/fun ./examples/cli_parse_example.fun --force bar xx x x --for=xxx --h=1):
1
Force enabled
Flags map:
{"force": 1, "for": xxx, "h": 1}
Positionals:
[bar, xx, x, x]
*/
