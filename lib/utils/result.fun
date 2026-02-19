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

// Result helpers: Ok(value) or Err(error)

// Constructors
fun ok(x)
  return {"_tag": "Ok", "value": x}

fun err(e)
  return {"_tag": "Err", "error": e}

// Predicates
fun is_ok(res)
  return res["_tag"] == "Ok"

fun is_err(res)
  return res["_tag"] == "Err"

// Extractors
fun unwrap(res)
  if is_ok(res)
    return res["value"]
  error("called unwrap() on Err: " + to_string(res["error"]))
  return nil

fun unwrap_or(res, default)
  if is_ok(res)
    return res["value"]
  return default

fun unwrap_err(res)
  if is_err(res)
    return res["error"]
  error("called unwrap_err() on Ok")
  return nil

// map(res, f) applies f to Ok value; Err passes through
fun result_map(res, f)
  if is_ok(res)
    return ok(f(res["value"]))
  return res

// map_err(res, f) applies f to error; Ok passes through
fun map_err(res, f)
  if is_err(res)
    return err(f(res["error"]))
  return res

// and_then(res, f) where f: a -> Result[b]
fun and_then(res, f)
  if is_ok(res)
    return f(res["value"]) // caller should return Result
  return res

// or_else(res, f) where f: () -> Result[a]
fun or_else(res, f)
  if is_ok(res)
    return res
  return f()

// to_option(res) -> Some(value) or None
fun to_option(res)
  if is_ok(res)
    return some(res["value"]) // requires option.fun loaded
  return none()
