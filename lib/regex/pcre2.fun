#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-11-25
 */

// PCRE2 stdlib abstraction wrapping VM pcre2_* builtins.
// Provides a small class with flags and user-friendly methods.

class PCRE2()
  fun i(this)
    return 1
  fun m(this)
    return 2
  fun s(this)
    return 4
  fun u(this)
    return 8
  fun x(this)
    return 16

  fun test(this, pattern, text, flags)
    if flags == nil
      flags = this.u()
    return pcre2_test(to_string(pattern), to_string(text), flags)

  fun match(this, pattern, text, flags)
    if flags == nil
      flags = this.u()
    return pcre2_match(to_string(pattern), to_string(text), flags)

  fun find_all(this, pattern, text, flags)
    if flags == nil
      flags = this.u()
    return pcre2_findall(to_string(pattern), to_string(text), flags)
