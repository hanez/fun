#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-04-02
 */

/*
 * Complex demonstration of nested functions that are local to their
 * enclosing function, without any variable capture (no closures).
 * All state is passed explicitly through parameters at each call.
 *
 * The example showcases:
 *  - Deeply nested helpers and multi-stage orchestration
 *  - Returning a nested function and using it later
 *  - Recursion implemented via a nested helper
 *  - Working with maps while threading state explicitly
 */

print("=== Complex nested functions (no captures) ===")

// 1) A small data pipeline on a user map, using local helpers
fun process_user(user)
  // Ensure a key is present; if missing, fill with default
  fun ensure_has(m, key, def)
    if has(m, key)
      return m
    m[key] = def
    return m

  // Normalize optional fields
  fun normalize_city(m)
    if has(m, "city")
      return m
    m["city"] = "Unknown"
    return m

  // Add derived attributes explicitly via parameters (no function calls on RHS)
  fun annotate(m)
    a = m["age"]
    // Inline age grouping without relying on an additional helper
    g = ""
    if a < 13
      g = "child"
    else if a < 20
      g = "teen"
    else if a < 65
      g = "adult"
    else
      g = "senior"
    m["group"] = g
    return m

  m1 = ensure_has(user, "name", "N/A")
  m2 = ensure_has(m1, "age", 0)
  m3 = normalize_city(m2)
  m4 = annotate(m3)
  return m4

u1 = {"name": "Ada", "age": 37}
u2 = process_user(u1)
print("process_user -> " + to_string(u2))

print("")
print("=== Returning a nested function (no captures) ===")

// 2) Return an inner function and use it later. The returned function
//    still requires all the data it needs as parameters (no implicit capture).
fun math_suite()
  fun addk(x, k)
    return x + k

  fun mulk(x, k)
    return x * k

  // Integer power via nested recursion helper
  fun powi(x, n)
    fun loop(acc, base, exp)
      if exp == 0
        return acc
      return loop(acc * base, base, exp - 1)
    return loop(1, x, n)

  // Compose a small arithmetic chain explicitly. Break into simple steps to
  // match parser expectations and avoid nested calls on the right-hand side.
  fun apply_chain(x, k_a, k_b, n)
    t1 = x + k_a
    t2 = t1 * k_b
    t3 = powi(t2, n)
    return t3

  return apply_chain

chain = math_suite()
print("chain(2, 3, 4, 2) -> expected ((2+3)*4)^2 = 400")
print(chain(2, 3, 4, 2))

print("")
print("=== Deep orchestration with 3-level nesting ===")

// 3) Multi-stage pipeline with explicit parameter threading through each level
fun orchestrate(a, b, c)
  fun stage1(x, a_, b_, c_)
    fun stage2(y, b2, c2)
      fun stage3(z, c3)
        // No capture: every value needed arrives via parameters
        tmp = z + c3
        return tmp * 2
      return stage3(y + b2, c2)
    return stage2(x + a_, b_, c_)

  // Kick off with x = 0 and thread a, b, c explicitly
  return stage1(0, a, b, c)

print("orchestrate(1, 2, 3) -> stage3((0+1)+2, 3) * 2 = (3+3)*2 = 12")
print(orchestrate(1, 2, 3))

print("")
print("=== Nested recursion: factorial via inner loop ===")

// 4) Factorial using an inner tail-recursive helper (no captures)
fun fact(n)
  fun go(i, acc)
    if i <= 1
      return acc
    return go(i - 1, acc * i)
  return go(n, 1)

print("fact(6) -> expected 720")
print(fact(6))

print("")
print("=== Higher-order style without captures ===")

// 5) Higher-order-like usage where the "strategy" function receives
//    all needed parameters explicitly.
fun reducer_sum_with_limit(x, limit)
  if x > limit
    return 0
  return x

fun fold3(a, b, c, f, p1, p2, p3)
  // Apply f to each and sum; f must accept (value, param)
  s1 = f(a, p1)
  s2 = f(b, p2)
  s3 = f(c, p3)
  return s1 + s2 + s3

print("fold3 with limit: (5<=10 ? 5 : 0) + (12<=10 ? 0 : 0) + (7<=10 ? 7 : 0) = 12")
print(fold3(5, 12, 7, reducer_sum_with_limit, 10, 10, 10))

/* Expected output:
=== Complex nested functions (no captures) ===
process_user -> {"age": 37, "city": "Unknown", "group": "adult", "name": "Ada"}

=== Returning a nested function (no captures) ===
chain(2, 3, 4, 2) -> expected ((2+3)*4)^2 = 400
400

=== Deep orchestration with 3-level nesting ===
orchestrate(1, 2, 3) -> stage3((0+1)+2, 3) * 2 = (3+3)*2 = 12
12

=== Nested recursion: factorial via inner loop ===
fact(6) -> expected 720
720

=== Higher-order style without captures ===
fold3 with limit: (5<=10 ? 5 : 0) + (12<=10 ? 0 : 0) + (7<=10 ? 7 : 0) = 12
12
*/
