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
    // Fallback: when running under a real CGI, POST data comes from stdin, not env.
    // Only attempt to read stdin if method is POST, content-length > 0, and POST_DATA is empty.
    if (!(typeof(pd) == "String" && len(pd) > 0))
      if (str_to_upper(this.env["REQUEST_METHOD"]) == "POST")
        cl = to_number(this.env["CONTENT_LENGTH"])
        if (cl > 0)
          // Read up to CONTENT_LENGTH bytes from stdin. Some CGI runners may deliver
          // urlencoded chunks split at '&' without including the delimiter; reconstruct
          // by inserting '&' between successive chunks.
          parts = []
          total = 0
          // Safeguard: cap iterations to avoid infinite loops on unexpected behavior
          iter = 0
          while (total < cl && iter < 100000)
            chunk = input("")
            if (!(typeof(chunk) == "String"))
              break
            if (len(chunk) == 0)
              // EOF without newline
              break
            push(parts, chunk)
            total = total + len(chunk)
            iter = iter + 1
          // Rebuild with '&' between pieces to restore typical form encoding
          if (len(parts) > 0)
            rebuilt = parts[0]
            idx = 1
            np = len(parts)
            while (idx < np)
              rebuilt = rebuilt + "&" + parts[idx]
              idx = idx + 1
            tmp = rebuilt
          else
            tmp = ""
          // Trim to declared content length to drop any stray newline added by the CGI runner
          if (len(tmp) > cl)
            pd = substr(tmp, 0, cl)
          else
            pd = tmp
          this.env["POST_DATA"] = pd
        else
          // No declared length; try to read a single line (common with simple runners)
          tmp = input("")
          if (typeof(tmp) == "String" && len(tmp) > 0)
            this.env["POST_DATA"] = tmp
          // Keep local variable in sync so parsing below sees the data
          pd = this.env["POST_DATA"]
    // Decide whether to treat body as urlencoded: default yes for POST unless explicitly multipart
    is_urlencoded = 1
    if (typeof(ct) == "String" && len(ct) > 0)
      lct = str_to_lower(ct)
      if (find(lct, "multipart/form-data") >= 0)
        is_urlencoded = 0
    if (is_urlencoded)
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

  // Translate CGI output (headers + body) into a full HTTP/1.1 response string
  // Input: raw string as emitted by a CGI script (e.g., via CGI.send()), containing
  // CGI headers followed by an empty line and then the body.
  // Output: a single HTTP/1.1 response string ready to be written to a socket.
  fun cgi_to_http_response(this, raw)
    out = to_string(raw)
    // Find header/body separator
    sep = find(out, "\r\n\r\n")
    seplen = 4
    if (sep < 0)
      sep = find(out, "\n\n")
      seplen = 2
    if (sep < 0)
      // No CGI headers, treat whole as body
      b = out
      resp = "HTTP/1.1 200 OK\r\n"
      resp = resp + "Content-Type: text/html; charset=utf-8\r\n"
      resp = resp + "Content-Length: " + to_string(len(b)) + "\r\n"
      resp = resp + "Connection: close\r\n\r\n" + b
      return resp

    header_str = substr(out, 0, sep)
    body = substr(out, sep + seplen, len(out) - sep - seplen)

    // Parse headers
    lines = str_split(header_str, "\n")
    code = 200
    text = "OK"
    // Accumulate headers (excluding Status)
    hh = []  // array of [k, v]
    i = 0
    n = len(lines)
    while (i < n)
      ln = str_trim(lines[i])
      if (len(ln) > 0)
        colon = find(ln, ":")
        if (colon > 0)
          k = str_trim(substr(ln, 0, colon))
          v = str_trim(substr(ln, colon + 1, len(ln) - colon - 1))
          if (str_to_upper(k) == "STATUS")
            // Expect like: 200 OK
            sp = find(v, " ")
            if (sp > 0)
              code = to_number(substr(v, 0, sp))
              text = str_trim(substr(v, sp + 1, len(v) - sp - 1))
            else
              code = to_number(v)
              if (code == 0) code = 200
              text = "OK"
          else
            // Keep other headers
            push(hh, [k, v])
      i = i + 1

    // Build HTTP response
    b = to_string(body)
    resp = "HTTP/1.1 " + to_string(code) + " " + text + "\r\n"
    // Emit collected headers
    j = 0
    m = len(hh)
    has_len = false
    while (j < m)
      p = hh[j]
      if (typeof(p) == "Array" && len(p) >= 2)
        hk = to_string(p[0])
        hv = to_string(p[1])
        if (str_to_upper(hk) == "CONTENT-LENGTH")
          has_len = true
        resp = resp + hk + ": " + hv + "\r\n"
      j = j + 1
    if (!has_len)
      resp = resp + "Content-Length: " + to_string(len(b)) + "\r\n"
    resp = resp + "Connection: close\r\n\r\n" + b
    return resp

  fun url_decode(this, s)
    source_str = to_string(s)
    decoded_result = ""
    number current_pos = 0
    number source_len = len(source_str)
    while (current_pos < source_len)
      current_char = substr(source_str, current_pos, 1)
      if (current_char == "+")
        decoded_result = decoded_result + " "
        current_pos = current_pos + 1
      else if (current_char == "%" && current_pos + 2 < source_len)
        hex_digit1 = substr(source_str, current_pos + 1, 1)
        hex_digit2 = substr(source_str, current_pos + 2, 1)
        hex_table = "0123456789ABCDEF"
        val1 = find(hex_table, str_to_upper(hex_digit1))
        val2 = find(hex_table, str_to_upper(hex_digit2))
        if (val1 >= 0 && val2 >= 0)
          number char_code = val1 * 16 + val2
          // Using a more robust table with explicit characters
          ascii_table = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
          if (char_code >= 32 && char_code <= 126)
            decoded_result = decoded_result + substr(ascii_table, char_code - 32, 1)
          else
            decoded_result = decoded_result + "%"
            decoded_result = decoded_result + hex_digit1
            decoded_result = decoded_result + hex_digit2
          current_pos = current_pos + 3
        else
          decoded_result = decoded_result + current_char
          current_pos = current_pos + 1
      else
        decoded_result = decoded_result + current_char
        current_pos = current_pos + 1
    return decoded_result

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
    // Manually scan so we can treat both '&' and ';' as pair separators (robust across environments)
    pairs = []
    buf = []
    number i = 0
    number n = len(src)
    while (i < n)
      ch = substr(src, i, 1)
      if (ch == "&" || ch == ";")
        push(pairs, join(buf, ""))
        buf = []
      else
        push(buf, ch)
      i = i + 1
    // tail
    push(pairs, join(buf, ""))

    j = 0
    lp = len(pairs)
    while (j < lp)
      kv = pairs[j]
      if (typeof(kv) == "String")
        token = kv
        // Trim CR/LF that might trail if body ended with a newline
        // Reuse str_trim which trims spaces and CR/LF
        token = str_trim(token)
        if (len(token) > 0)
          eq = find(token, "=")
          if (eq >= 0)
            k = substr(token, 0, eq)
            v = substr(token, eq + 1, len(token) - eq - 1)
          else
            k = token
            v = ""
          key = this.url_decode(k)
          val = this.url_decode(v)
          push(out, [key, val])
      j = j + 1
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
