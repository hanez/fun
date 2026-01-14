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
 * Demonstrates curl_get and JSON.parse working together.
 */

url = "https://httpbin.org/json"
resp = curl_get(url)
print("Raw length: " + to_string(len(resp)))

// If JSON support is enabled, parse it
obj = json_parse(resp)
if obj != nil
  print("Title: " + obj["slideshow"]["title"])

/* Expected output:
Raw length: 429
Title: Sample Slide Show
*/

