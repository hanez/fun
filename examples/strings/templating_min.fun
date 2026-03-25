#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-03-25
 */

// Micro templating: replace {{key}} with map values
#include <strings.fun>
tpl = "Hello, {{name}} from {{city}}!"
ctx = {"name": "Fun", "city": "Cyberspace"}

fun render(template, data)
  out = to_string(template)
  for k in keys(data)
    needle = "{{" + to_string(k) + "}}"
    out = str_replace_all(out, needle, to_string(data[k]))
  return out

print(render(tpl, ctx))

/* Expected output:
Hello, Fun from Cyberspace!
*/
