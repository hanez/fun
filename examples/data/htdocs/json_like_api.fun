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
cgi.content_type("application/json; charset=utf-8")
name = cgi.param("name")
if (len(name) == 0) name = "World"
body = "{\n  \"greeting\": \"Hello, " + cgi.escape_html(name) + "!\",\n  \"method\": \"" + cgi.escape_html(cgi.env["REQUEST_METHOD"]) + "\"\n}"
cgi.send(body)
