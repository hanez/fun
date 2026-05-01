/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file fun_version.c
 * @brief VM opcode snippet: push the current Fun version string (OP_FUN_VERSION).
 *
 * Included into vm.c dispatch. Pushes a string constant with the build's
 * FUN_VERSION onto the VM stack.
 */

// Pushes the current Fun version string onto the stack.

case OP_FUN_VERSION: {
#ifndef FUN_VERSION
#define FUN_VERSION "0.0.0-dev"
#endif
  push_value(vm, make_string(FUN_VERSION));
  break;
}
