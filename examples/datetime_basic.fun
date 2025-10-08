#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-04
 */

// Date/time opcode usage examples
#include <utils/datetime.fun>

fun main()
  dt = DateTime()

  ms = dt.now_ms()
  print(join(["now ms: ", to_string(ms)], ""))

  iso = dt.iso_now()
  print(join(["iso now: ", iso], ""))

  // Formatting an arbitrary timestamp (epoch ms)
  print(dt.format(ms, "%Y-%m-%d %H:%M:%S %Z"))

  // Measuring elapsed time with monotonic clock
  start = dt.mono_ms()

  sleep(100)
  sleep(10)
  elapsed = dt.mono_ms() - start
  print(join(["elapsed ~100ms => ", to_string(elapsed), " ms"], ""))

// Invoke main when script is executed directly
main()

/* Possible output:
now ms: 1759531591326
iso now: 2025-10-04T00:46:31
2025-10-04 00:46:31 CEST
elapsed ~100ms => 110 ms
*/
