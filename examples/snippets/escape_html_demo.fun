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
raw = "<script>alert('xss') & more</script>"
print("raw:    " + raw)
print("escaped:" + cgi.escape_html(raw))

/* Expected output:
raw:    <script>alert('xss') & more</script>
escaped:&lt;script&gt;alert(&#39;xss&#39;) &amp; more&lt;/script&gt;
*/
