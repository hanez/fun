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
v = to_number(cgi.cookie("visits"))
if (v <= 0) v = 0
v = v + 1
cgi.header("Set-Cookie", "visits=" + to_string(v) + "; Path=/; HttpOnly")
cgi.content_type("text/html; charset=utf-8")
body = "<html><body><p>Visits: " + to_string(v) + "</p></body></html>"
cgi.send(body)

/* Expected output (first visit, no existing cookie):
Status: 200 OK
Content-Type: text/html; charset=utf-8
Set-Cookie: visits=1; Path=/; HttpOnly

<html><body><p>Visits: 1</p></body></html>
*/
