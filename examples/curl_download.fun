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
 * Demonstrates curl_download saving a file to disk.
 */

url = "https://httpbin.org/image/png"
path = "./downloaded.png"
ok = curl_download(url, path)
if ok == 1
  print("Downloaded to " + path)
else
  print("Download failed")
