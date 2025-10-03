/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-04
 */

// Regex stdlib abstraction over VM regex opcodes.
// Provides simple methods and sensible defaults.

/*
Methods:
- match(text, pattern) -> 1/0 (full match)
- search(text, pattern) -> map {"match": str, "start": int, "end": int, "groups": array}
- replace(text, pattern, repl) -> string (global)
*/

class Regex()
  // Full match: returns 1 if the whole string matches the pattern
  fun match(this, text, pattern)
    return regex_match(to_string(text), to_string(pattern))

  // First match with groups as array
  fun search(this, text, pattern)
    return regex_search(to_string(text), to_string(pattern))

  // Global replace; replacement is literal (no backrefs)
  fun replace(this, text, pattern, repl)
    return regex_replace(to_string(text), to_string(pattern), to_string(repl))
