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

// Extended Date/Time examples using the stdlib DateTime class
#include <utils/datetime.fun>

fun main()
  dt = DateTime()

  print("--- Basics ---")
  now_ms = dt.now_ms()
  print(join(["now_ms: ", to_string(now_ms)], ""))
  print(join(["now_s: ", to_string(dt.now_s())], ""))
  print(join(["iso_now: ", dt.iso_now()], ""))
  print(join(["today: ", dt.today_str()], ""))

  print("--- Formatting helpers ---")
  print(join(["iso_from(now): ", dt.iso_from(now_ms)], ""))
  print(join(["date_str(now): ", dt.date_str(now_ms)], ""))
  print(join(["time_str(now): ", dt.time_str(now_ms)], ""))

  print("--- Conversions ---")
  print(join(["ms_to_s(1234): ", to_string(dt.ms_to_s(1234))], ""))
  print(join(["s_to_ms(2): ", to_string(dt.s_to_ms(2))], ""))

  print("--- Arithmetic ---")
  in_2s = dt.add_seconds(now_ms, 2)
  print(join(["in 2s (ms): ", to_string(in_2s)], ""))
  print(join(["diff_ms(now, in_2s): ", to_string(dt.diff_ms(now_ms, in_2s))], ""))

  print("--- Timer ---")
  t0 = dt.start_timer()
  dt.sleep_ms(120)
  print(join(["elapsed ~120ms: ", to_string(dt.elapsed_ms(t0)), " ms"], ""))

main()

/* Possible output:
--- Basics ---
now_ms: 1765320472780
now_s: 1765320472
iso_now: 2025-12-09T23:47:52
today: 2025-12-09
--- Formatting helpers ---
iso_from(now): 2025-12-09T23:47:52
date_str(now): 2025-12-09
time_str(now): 23:47:52
--- Conversions ---
ms_to_s(1234): 1
s_to_ms(2): 2000
--- Arithmetic ---
in 2s (ms): 1765320474780
diff_ms(now, in_2s): 2000
--- Timer ---
elapsed ~120ms: 120 ms
*/
