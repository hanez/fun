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

// Option helpers for ergonomic error-absent values.
// Represented as dictionaries with a _tag and optional value field.

// Constructors
fun some(x)
  return {"_tag": "Some", "value": x}

fun none()
  return {"_tag": "None"}

// Predicates
fun is_some(opt)
  return opt["_tag"] == "Some"

fun is_none(opt)
  return opt["_tag"] == "None"

// Extractors
fun unwrap(opt)
  if is_some(opt)
    return opt["value"]
  // Fall back to raising a runtime error
  error("called unwrap() on None")
  return nil

fun unwrap_or(opt, default)
  if is_some(opt)
    return opt["value"]
  return default

// Functional helpers
// map(opt, f) -> Some(f(value)) or None
fun option_map(opt, f)
  if is_some(opt)
    return some(f(opt["value"]))
  return none()

// and_then(opt, f) where f: a -> Option[b]
fun and_then(opt, f)
  if is_some(opt)
    return f(opt["value"])
  return none()

// or_else(opt, f) where f: () -> Option[a]
fun or_else(opt, f)
  if is_some(opt)
    return opt
  return f()

// to_result(opt, err)
fun to_result(opt, err)
  if is_some(opt)
    return ok(opt["value"])
  return err(err)

// Convenience: try_get(dict, key) -> Option[value]
fun try_get(dict, key)
  if has_key(dict, key)
    return some(dict[key])
  return none()
