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
// Build HTML with a form at the top to submit POST data back to this script
html = "<html><body>"
html = html + "<h1>Submit data</h1>"
html = html + "<form method=\"POST\" action=\"\">"
html = html + "<label>A: <input type=\"text\" name=\"a\" value=\"1\"></label><br>"
html = html + "<label>B: <input type=\"text\" name=\"b\" value=\"2\"></label><br>"
html = html + "<label>C: <input type=\"text\" name=\"c\" value=\"3\"></label><br>"
html = html + "<button type=\"submit\">Submit</button>"
html = html + "</form>"
html = html + "<h1>POST fields</h1><ul>"

// Always show fields a, b, and c (print names even if no value was submitted)
names = ["a", "b", "c"]
i_name = 0
n_names = len(names)
while (i_name < n_names)
  k = names[i_name]
  vals = cgi.param_all(k)
  i = 0
  n = len(vals)
  joined = ""
  while (i < n)
    if (i > 0) joined = joined + ", "
    joined = joined + cgi.escape_html(vals[i])
    i = i + 1
  html = html + "<li><b>" + cgi.escape_html(k) + "</b>: " + joined + "</li>"
  i_name = i_name + 1
html = html + "</ul></body></html>"
cgi.send(html)

/* Possible output (POST with body: "a=1&b=2&c=3"):
Status: 200 OK
Content-Type: text/html; charset=utf-8

<html><body><h1>POST fields</h1><ul><li><b>a</b>: 1</li><li><b>b</b>: b, 2</li><li><b>c</b>: 3</li></ul></body></html>
*/
