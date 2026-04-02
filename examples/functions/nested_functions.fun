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

print("=== Nested functions (local to outer function) ===")

// Outer function defines two inner helpers that are only usable inside outer
fun outer(a, b)
  // Defined inside outer; not visible as a global symbol
  fun times3(x)
    return x * 3
  end

  // Another local helper; also only visible within outer
  fun sum_plus1(x, y)
    return x + y + 1
  end

  // Use the local helpers
  t1 = times3(a)
  t2 = times3(b)
  return sum_plus1(t1, t2)
end

print("outer(2, 5) -> expected 2*3 + 5*3 + 1 = 22")
print(outer(2, 5))

print("")
print("=== Deeper nesting without variable capture ===")

// Demonstrates function-inside-function-inside-function.
// Each level avoids referencing outer locals directly; values
// are threaded through parameters instead.
fun demo_deep(n)
  fun one(x)
    fun two(y)
      fun three(z)
        return z + 1
      end
      return three(y) + 1
    end
    return two(x) + 1
  end
  return one(n)
end

print("demo_deep(4) -> expected 7")
print(demo_deep(4))

/*
Expected output:
=== Nested functions (local to outer function) ===
outer(2, 5) -> expected 2*3 + 5*3 + 1 = 22
22

=== Deeper nesting without variable capture ===
demo_deep(4) -> expected 7
7
*/
