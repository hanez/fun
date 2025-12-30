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

// Demonstrates accessing raw script arguments via lib/cli.fun
// Usage:
//   FUN_LIB_DIR="$(pwd)/lib" ./build_debug/fun ./examples/cli_argv_dump.fun -f --force=42 foo bar

#include <cli.fun>

args = argv()

print(join(["FUN_ARGC=", env("FUN_ARGC")], ""))
print(join(["FUN_ARGS='", env("FUN_ARGS"), "'"], ""))
print("Raw argv array:")
print(args)

/* Expected output:
FUN_ARGC=3
FUN_ARGS='-f --force --force=42'
Raw argv array:
[-f, --force, --force=42
*/
