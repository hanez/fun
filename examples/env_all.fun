#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-12-28
 */

all_env = env_all()
keys = keys(all_env)

print("Number of environment variables: " + to_string(len(keys)))
print("--------------------------------------------------")

i = 0
while i < len(keys)
  key = keys[i]
  print(key + "=" + all_env[key])
  i = i + 1

/* Possible output:
Depends on your environment.
*/
