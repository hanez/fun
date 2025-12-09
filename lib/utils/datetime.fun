/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

// Date/time utilities abstraction for the Fun stdlib.
// Minimal version to validate syntax.

/*
 * Date/time utilities abstraction for the Fun stdlib.
 * Provides a convenient class-based API over VM date/time opcodes.
 *
 * Methods:
 *  - now_ms()    -> current wall clock ms since Unix epoch
 *  - mono_ms()   -> monotonic clock ms (for measuring intervals)
 *  - format(ms, fmt) -> format an epoch-ms timestamp using strftime format
 *  - iso_now()   -> ISO-8601 like local timestamp (YYYY-MM-DDTHH:MM:SS)
 */

class DateTime()
  // Current wall-clock time in milliseconds since Unix epoch
  fun now_ms(this)
    return time_now_ms()

  // Monotonic clock in milliseconds
  fun mono_ms(this)
    return clock_mono_ms()

  // Format an epoch-ms timestamp with a strftime() format string
  fun format(this, ms, fmt)
    // ensure argument order and types are okay
    return date_format(to_number(ms), to_string(fmt))

  // Convenience: ISO-like current datetime string (local time)
  fun iso_now(this)
    ms = time_now_ms()
    return date_format(ms, "%Y-%m-%dT%H:%M:%S")

  // Seconds since Unix epoch (integer)
  fun now_s(this)
    return to_number(time_now_ms() / 1000)

  // Convert milliseconds to seconds (floor)
  fun ms_to_s(this, ms)
    return to_number(to_number(ms) / 1000)

  // Convert seconds to milliseconds
  fun s_to_ms(this, s)
    return to_number(to_number(s) * 1000)

  // Add milliseconds to an epoch-ms timestamp
  fun add_ms(this, ms, delta_ms)
    return to_number(to_number(ms) + to_number(delta_ms))

  // Add seconds to an epoch-ms timestamp
  fun add_seconds(this, ms, seconds)
    return this.add_ms(ms, this.s_to_ms(seconds))

  // Difference in milliseconds: b - a
  fun diff_ms(this, a_ms, b_ms)
    return to_number(to_number(b_ms) - to_number(a_ms))

  // Milliseconds elapsed since given epoch-ms timestamp
  fun since_ms(this, past_ms)
    return this.diff_ms(past_ms, this.now_ms())

  // Format an arbitrary epoch-ms timestamp as ISO local
  fun iso_from(this, ms)
    return date_format(to_number(ms), "%Y-%m-%dT%H:%M:%S")

  // Date-only string for a timestamp (YYYY-MM-DD)
  fun date_str(this, ms)
    return date_format(to_number(ms), "%Y-%m-%d")

  // Time-only string for a timestamp (HH:MM:SS)
  fun time_str(this, ms)
    return date_format(to_number(ms), "%H:%M:%S")

  // Today's date as YYYY-MM-DD
  fun today_str(this)
    return this.date_str(this.now_ms())

  // Start a monotonic timer
  fun start_timer(this)
    return clock_mono_ms()

  // Elapsed ms from a monotonic start value
  fun elapsed_ms(this, start_mono_ms)
    return to_number(clock_mono_ms() - to_number(start_mono_ms))

  // Sleep for the given milliseconds (non-negative)
  fun sleep_ms(this, ms)
    m = to_number(ms)
    if m > 0
      sleep(m)
    return m

  // Sleep for the given seconds
  fun sleep_s(this, s)
    return this.sleep_ms(this.s_to_ms(s))
