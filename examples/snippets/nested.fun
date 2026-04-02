#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Demonstrates nested functions that are only visible inside the
 * outer function where they are defined. This example avoids
 * capturing outer variables (closures) and instead passes values
 * explicitly, which works with the current implementation.
 *
 * Added: 2026-04-02
 */

fun outer(x)
  fun inner(y)
    return y * 2
  end
  return inner(x) + inner(3)
end

print(outer(5))
