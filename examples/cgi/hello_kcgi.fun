#!/usr/bin/env fun

/*
 * Minimal example using KCGI wrapper.
 * Run under a CGI/FastCGI environment where kcgi can parse the request.
 */

#include <net/kcgi.fun>

cgi = KCGI()
name = cgi.get_or("name", "Fun")

body = "<h1>Hello, " + to_string(name) + "!</h1>"
cgi.reply(200, "text/html; charset=utf-8", body)
