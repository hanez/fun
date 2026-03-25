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

fun assert_like(cond, msg)
  if (!cond)
    print("ASSERT FAILED: " + to_string(msg))
    return 0
  return 1

ok = assert_like(1 + 1 == 2, "math broke")
print("assert returned: " + to_string(ok))

/* Expected output:
assert returned: 1
*/
