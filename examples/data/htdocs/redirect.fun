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
cgi.redirect("/hello.fun?name=Fun", 302)
cgi.content_type("text/html; charset=utf-8")
cgi.send("<html><body>Redirecting...</body></html>")
