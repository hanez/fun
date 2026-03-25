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
// Render parsed CGI params as key => value(s)
pmap = cgi.params()
html = html + "<h2>Params</h2><ul>"
for k in keys(pmap)
  vals = cgi.param_all(k)
  // Join multiple values with ", "
  i = 0
  n = len(vals)
  joined = ""
  while (i < n)
    if (i > 0)
      joined = joined + ", "
    joined = joined + cgi.escape_html(vals[i])
    i = i + 1
  html = html + "<li><b>" + cgi.escape_html(k) + "</b>: " + joined + "</li>"
html = html + "</ul>"
html = html + "</body></html>"

// Send CGI headers + body
cgi.send(html)

/* Expected output (GET /hello.fun?name=Fun with no ua cookie):
Status: 200 OK
Content-Type: text/html; charset=utf-8
Set-Cookie: ua=FunClient; Path=/; HttpOnly

<html><head><title>Fun CGI</title></head><body><h1>Hello, Fun!</h1><p>REQUEST_METHOD: GET</p><p>QUERY_STRING: name=Fun</p><p>User-Agent cookie: </p><h2>Params</h2><ul><li><b>name</b>: Fun</li></ul></body></html>
*/
