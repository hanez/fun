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

// Minimal, parser-friendly CGI helper (incrementally extend as needed)

#include <strings.fun>

class CGI()
  fun _construct(this)
    this._params = {}
    this._cookies = {}
    this._headers = {}
    this._header_list = []  // preserve insertion order for emission without map iteration
    this._status = 200
    this._status_text = "OK"
    this._content_type = "text/html; charset=utf-8"

    this.env = {}
    this.env["REQUEST_METHOD"] = env("REQUEST_METHOD")
    this.env["QUERY_STRING"] = env("QUERY_STRING")
    this.env["CONTENT_TYPE"] = env("CONTENT_TYPE")
    this.env["CONTENT_LENGTH"] = env("CONTENT_LENGTH")
    this.env["HTTP_COOKIE"] = env("HTTP_COOKIE")
    this.env["POST_DATA"] = env("POST_DATA")

    // Cookies
    this._cookies = this._parse_cookies(this.env["HTTP_COOKIE"])

    // Params from QUERY_STRING
    qs = this.env["QUERY_STRING"]
    if (typeof(qs) == "String" && len(qs) > 0)
      parsed_qs = this._parse_urlencoded(qs)
      this._merge_params(parsed_qs)

    // Params from POST (x-www-form-urlencoded only)
    ct = this.env["CONTENT_TYPE"]
    pd = this.env["POST_DATA"]
    if (typeof(ct) == "String" && len(ct) > 0 && find(str_to_lower(ct), "application/x-www-form-urlencoded") >= 0)
      if (typeof(pd) == "String" && len(pd) > 0)
        parsed_pd = this._parse_urlencoded(pd)
        this._merge_params(parsed_pd)

  fun param(this, name)
    arr = this._params[name]
    if (typeof(arr) != "Array")
      return ""
    if (len(arr) > 0)
      return arr[0]
    return ""

  fun param_all(this, name)
    arr = this._params[name]
    if (typeof(arr) != "Array")
      return []
    return arr

  fun params(this)
    // Return the internal map directly (no copying, to avoid map iteration)
    return this._params

  fun cookie(this, name)
    c = this._cookies[name]
    if (typeof(c) != "String")
      return ""
    return c

  fun cookies(this)
    // Return the internal cookies map directly
    return this._cookies

  fun status(this, code, text)
    this._status = to_number(code)
    if (len(text) > 0)
      this._status_text = text

  fun content_type(this, ct)
    this._content_type = to_string(ct)

  fun header(this, name, value)
    k = to_string(name)
    v = to_string(value)
    this._headers[k] = v
    // Track in ordered list for emission
    push(this._header_list, [k, v])

  fun redirect(this, location, code)
    c = to_number(code)
    if (c == 0)
      c = 302
    if (c == 301)
      this.status(c, "Moved Permanently")
    else
      this.status(c, "Found")
    this.header("Location", to_string(location))

  fun header_str(this)
    out = "Status: " + to_string(this._status) + " " + this._status_text + "\r\n"
    out = out + "Content-Type: " + this._content_type + "\r\n"
    // Emit headers from the ordered list
    i = 0
    n = len(this._header_list)
    while (i < n)
      pair = this._header_list[i]
      if (typeof(pair) == "Array" && len(pair) >= 2)
        out = out + to_string(pair[0]) + ": " + to_string(pair[1]) + "\r\n"
      i = i + 1
    out = out + "\r\n"
    return out

  fun send(this, body)
    print(this.header_str() + to_string(body))

  fun escape_html(this, s)
    a = to_string(s)
    a = str_replace_all(a, "&", "&amp;")
    a = str_replace_all(a, "<", "&lt;")
    a = str_replace_all(a, ">", "&gt;")
    a = str_replace_all(a, "\"", "&quot;")
    a = str_replace_all(a, "'", "&#39;")
    return a

  // Minimal url-decoder: '+' -> space; %XX for ASCII printable
  fun url_decode(this, s)
    src = to_string(s)
    // Simplified for parser-compatibility: only translate '+' to space
    return str_replace_all(src, "+", " ")

  fun _merge_params(this, pairs)
    // pairs: array of [key, value] entries
    if (typeof(pairs) != "Array")
      return 0
    i = 0
    n = len(pairs)
    while (i < n)
      p = pairs[i]
      if (typeof(p) == "Array" && len(p) >= 2)
        key = to_string(p[0])
        val = to_string(p[1])
        a = this._params[key]
        if (typeof(a) != "Array")
          this._params[key] = []
        push(this._params[key], val)
      i = i + 1
    return 1

  fun _parse_urlencoded(this, s)
    out = []   // array of [key, value]
    src = to_string(s)
    if (len(src) == 0)
      return out
    parts = str_split(src, "&")
    i = 0
    lp = len(parts)
    while (i < lp)
      kv = parts[i]
      if (typeof(kv) == "String" && len(kv) > 0)
        eq = find(kv, "=")
        if (eq >= 0)
          k = substr(kv, 0, eq)
          v = substr(kv, eq + 1, len(kv) - eq - 1)
        else
          k = kv
          v = ""
        key = this.url_decode(k)
        val = this.url_decode(v)
        push(out, [key, val])
      i = i + 1
    return out

  fun _parse_cookies(this, cookie_str)
    out = {}
    if (len(cookie_str) == 0)
      return out
    semi = str_split(cookie_str, ";")
    i = 0
    while (i < len(semi))
      part = str_trim(semi[i])
      if (len(part) > 0)
        eq = find(part, "=")
        if (eq >= 0)
          k = str_trim(substr(part, 0, eq))
          v = str_trim(substr(part, eq + 1, len(part) - eq - 1))
          out[k] = v
      i = i + 1
    return out
