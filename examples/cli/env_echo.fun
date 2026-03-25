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

keys_to_show = ["HOME", "USER", "SHELL", "PATH"]
for k in keys_to_show
  print(k + "=" + env(k))

/* Expected output (values depend on your environment):
HOME=/home/youruser
USER=youruser
SHELL=/bin/bash
PATH=/usr/local/bin:... (truncated)
*/
