#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-12-09
 */

// Simple stopwatch using DateTime helpers
#include <utils/datetime.fun>

fun main()
  dt = DateTime()
  print("Starting timer for ~250ms ...")
  t0 = dt.start_timer()
  dt.sleep_s(0.2) // 200 ms
  dt.sleep_ms(50)
  elapsed = dt.elapsed_ms(t0)
  print(join(["Elapsed: ", to_string(elapsed), " ms"], ""))

main()

/* Possible output:
Starting timer for ~250ms ...
Elapsed: 250 ms
*/
