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

/*
 * Demonstrates curl_post sending form data and printing response.
 */

url = "https://httpbin.org/post"
data = "name=Fun&lang=fun"
resp = curl_post(url, data)
print("Response: " + resp)

// If JSON support is enabled, parse it
obj = json_parse(resp)
if obj != nil
  print("Content-Type: " + to_string(obj["headers"]["Content-Type"]))

if obj != nil
  print("Host: " + to_string(obj["headers"]["Host"]))

if obj != nil
  print("Origin: " + to_string(obj["origin"]))
