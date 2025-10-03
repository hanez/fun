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
