#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-02-19
 */

/*
 * Error handling examples using Result and Option helpers.
 */

#include <utils/option.fun>
#include <utils/result.fun>

// Simulate a fallible parse
fun parse_int(s)
  if regex_match("^[+-]?[0-9]+$", s)
    return ok(to_number(s))
  return err("invalid integer: " + s)

// Divide two integers, handling division by zero
fun safe_div(a, b)
  if b == 0
    return err("division by zero")
  return ok(a / b)

// Option example: get environment variable
fun get_env_opt(name)
  v = env(name)
  if v == nil
    return none()
  if v == ""
    return none()
  return some(v)

print("-- Result examples --")
r = parse_int("123")
print("parse_int 123 ok? " + to_string(is_ok(r)) + ", value: " + to_string(unwrap_or(r, -1)))

q = parse_int("x12")
print("parse_int x12 ok? " + to_string(is_ok(q)) + ", or default 0: " + to_string(unwrap_or(q, 0)))

print("chained and_then: '10' / '2'")
// Simpler explicit flow without nested anonymous functions
a = parse_int("10")
b = parse_int("2")
if is_ok(a) && is_ok(b)
  res = safe_div(unwrap(a), unwrap(b))
else
  if is_err(a)
    res = a
  else
    res = b
print("=> ok? " + to_string(is_ok(res)) + ", value: " + to_string(unwrap_or(res, -1)))

print("-- Option examples --")
home = get_env_opt("HOME")
print("HOME is set? " + to_string(is_some(home)) + ", value: " + to_string(unwrap_or(home, "<none>")))
