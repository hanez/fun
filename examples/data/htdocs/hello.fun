#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-03-25
 */

#include <net/cgi.fun>

cgi = CGI()

name = cgi.param("name")
if (len(name) == 0)
  name = "World"

ua = cgi.cookie("ua")
if (len(ua) == 0)
  // Set a demo cookie via header()
  cgi.header("Set-Cookie", "ua=FunClient; Path=/; HttpOnly")

cgi.content_type("text/html; charset=utf-8")

html = ""
html = html + "<html><head><title>Fun CGI</title></head><body>"
html = html + "<h1>Hello, " + cgi.escape_html(name) + "!</h1>"
html = html + "<p>REQUEST_METHOD: " + cgi.escape_html(cgi.env["REQUEST_METHOD"]) + "</p>"
html = html + "<p>QUERY_STRING: " + cgi.escape_html(cgi.env["QUERY_STRING"]) + "</p>"
html = html + "<p>User-Agent cookie: " + cgi.escape_html(ua) + "</p>"
html = html + "<h2>Params</h2><pre>" + to_string(cgi.params()) + "</pre>"
html = html + "</body></html>"

// Send CGI headers + body
cgi.send(html)
