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
cgi.content_type("text/html; charset=utf-8")
pmap = cgi.params()
html = "<html><body><h1>POST fields</h1><ul>"
for k in keys(pmap)
  vals = cgi.param_all(k)
  i = 0; n = len(vals); joined = ""
  while (i < n)
    if (i > 0) joined = joined + ", "
    joined = joined + cgi.escape_html(vals[i])
    i = i + 1
  html = html + "<li><b>" + cgi.escape_html(k) + "</b>: " + joined + "</li>"
html = html + "</ul></body></html>"
cgi.send(html)

/* Expected output (POST with body: "a=1&b=two&b=2"):
Status: 200 OK
Content-Type: text/html; charset=utf-8

<html><body><h1>POST fields</h1><ul><li><b>a</b>: 1</li><li><b>b</b>: two, 2</li></ul></body></html>
*/
