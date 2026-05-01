/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file env_all.c
 * @brief Implements OP_ENV_ALL to read the full environment into a map.
 *
 * Behavior:
 * - Pushes a new map where each key is an environment variable and each value is its string value.
 * - Keys and values are copied; the caller owns the returned map Value.
 */

case OP_ENV_ALL: {
  extern char **environ;
  Value m = make_map_empty();
  if (environ) {
    for (char **env = environ; *env; ++env) {
      char *entry = *env;
      char *equals = strchr(entry, '=');
      if (equals) {
        size_t key_len = equals - entry;
        char *key = malloc(key_len + 1);
        if (key) {
          memcpy(key, entry, key_len);
          key[key_len] = '\0';
          map_set(&m, key, make_string(equals + 1));
          free(key);
        }
      }
    }
  }
  push_value(vm, m);
  break;
}
