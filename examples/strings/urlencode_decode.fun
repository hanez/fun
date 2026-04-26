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

#include <net/cgi.fun>

cgi = CGI()
s = "name=Fun+Lang&msg=Hello%2C+World%21"
print("raw:   " + s)
print("decod: " + cgi.url_decode(s))

/* Expected output:
raw:   name=Fun+Lang&msg=Hello%2C+World%21
decod: name=Fun Lang&msg=Hello, World!

*/
