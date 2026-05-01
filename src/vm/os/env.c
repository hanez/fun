/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file env.c
 * @brief Implements OP_ENV to read an environment variable by name.
 *
 * Behavior:
 * - Pops a string key from the stack and pushes the associated environment value as string.
 * - If the variable is not set, pushes an empty string ("") rather than Nil.
 *
 * Errors:
 * - If the key is not a string, prints an error and terminates the VM with exit(1).
 */

case OP_ENV: {
  Value key = pop_value(vm);
  if (key.type != VAL_STRING) {
    fprintf(stderr, "Runtime type error: ENV expects string name\n");
    free_value(key);
    exit(1);
  }
  const char *name = key.s ? key.s : "";
  const char *val = getenv(name);
  /* Return empty string if not set (consistent with read_file fallback style) */
  push_value(vm, make_string(val ? val : ""));
  free_value(key);
  break;
}
