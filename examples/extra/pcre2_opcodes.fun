#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-11-25
 */

// PCRE2 example using VM builtins directly (no class wrapper)
// Requires building Fun with -DFUN_WITH_PCRE2=ON

print("-- PCRE2 builtins (no class) --")

pattern = "(\\w+)"    // capture a word
text = "Hello 123 world"

// Flags: 1=I, 2=M, 4=S, 8=U (UTF), 16=X; we’ll use UTF by default
flags = 8

print("test:")
print(pcre2_test(pattern, text, flags))

m = pcre2_match(pattern, text, flags)
if (m != nil)
  print("first full:")
  print(m["full"])
  print("span:")
  print(m["start"])
  print("..")
  print(m["end"])
  print("groups count:")
  print(len(m["groups"]))

all = pcre2_findall("\\w+", text, flags)
for x in all
  print("all:")
  print(x["full"])
  print("@")
  print(x["start"])
  print("..")
  print(x["end"])

print("")
print("-- More regex demos --")

// Helper: OR flags (uses VM bor opcode)
fun OR(a, b)
  return bor(a, b)

// Demo 1: E-mail extraction (case-insensitive)
email_text = "E-mails: one@example.com, Two@Example.COM; invalid: x@y"
email_pat = "[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}"
email_flags = OR(flags, 1)   // UTF | I
print("Emails (findall):")
emails = pcre2_findall(email_pat, email_text, email_flags)
for e in emails
  print(e["full"])

// Demo 2: URLs (very simple, for demo purposes)
url_text = "See http://example.com and https://fun-lang.xyz/docs?x=1#top"
url_pat = "https?://[A-Za-z0-9._~:/?#[@]!$&'()*+,;=%-]+"
print("URLs:")
for u in pcre2_findall(url_pat, url_text, flags)
  print(u["full"])

// Demo 3: IPv4 addresses
ip_text = "ping 8.8.8.8 and 192.168.0.1; not 999.999.999.999"
ip_pat = "(?:(?:25[0-5]|2[0-4]\\d|1?\\d?\\d)\\.){3}(?:25[0-5]|2[0-4]\\d|1?\\d?\\d)"
print("IPv4:")
for ip in pcre2_findall(ip_pat, ip_text, flags)
  print(ip["full"])

// Demo 4: Dates (YYYY-MM-DD)
date_text = "Born 1999-12-31, updated 2025-11-25, bad 2025-13-40"
date_pat = "(\\d{4})-(0[1-9]|1[0-2])-(0[1-9]|[12]\\d|3[01])"
print("Dates (with groups Y/M/D):")
for d in pcre2_findall(date_pat, date_text, flags)
  print(d["full"])
  print(join(d["groups"], "/"))

// Demo 5: Hex colors (#RRGGBB)
color_text = "Palette: #FF00FF, #1a2b3c, not #abcd or #12345g"
color_pat = "#[0-9A-Fa-f]{6}"
print("Hex colors:")
for c in pcre2_findall(color_pat, color_text, flags)
  print(c["full"])

// Demo 6: Quoted strings with escapes
q_text = 'say "hi there" and "indented" plus "quote\\"inside"'
q_pat = '"([^"\\\\]|\\\\.)*"'
print("Quoted strings (with escapes):")
for q in pcre2_findall(q_pat, q_text, flags)
  print(q["full"])

// Demo 7: Multiline anchors with /m (M flag)
ml_text = "first line\nSecond line\nthird"
ml_pat = "^(\\w+)"
ml_flags = OR(flags, 2) // UTF | M
print("Multiline ^ anchors (first token of each line):")
for ml in pcre2_findall(ml_pat, ml_text, ml_flags)
  print(ml["full"])

// Demo 8: Dotall vs non-dotall
ds_text = "BEGIN\nline1\nline2\nEND"
pat_nd = "BEGIN.*END"          // default: . does not match newlines
pat_ds = "BEGIN.*END"          // with DOTALL, it does
print("Dotall OFF (should fail):")
print(pcre2_test(pat_nd, ds_text, flags))
print("Dotall ON (should match):")
print(pcre2_test(pat_ds, ds_text, OR(flags, 4)))

// Demo 9: Word boundaries and case-insensitive find
wb_text = "The theater and the THE can differ."
wb_pat = "\\bthe\\b"
print("Word boundary, case-insensitive:")
for w in pcre2_findall(wb_pat, wb_text, OR(flags, 1))
  print(w["full"])

// Demo 10: Lookahead — word followed by number
la_text = "foo 123, bar, baz 9"
la_pat = "\\w+(?=\\s+\\d+)"
print("Lookahead (word before number):")
for a in pcre2_findall(la_pat, la_text, flags)
  print(a["full"])

// Demo 11: Non-greedy vs greedy
ng_text = "<a>one</a><a>two</a>"
greedy = "<a>.*</a>"
lazy   = "<a>.*?</a>"
print("Greedy:")
for g in pcre2_findall(greedy, ng_text, OR(flags, 4))  // DOTALL ensures '.' covers any
  print(g["full"])
print("Non-greedy:")
for l in pcre2_findall(lazy, ng_text, OR(flags, 4))
  print(l["full"])

/* Expected output:
-- PCRE2 builtins (no class) --
test:
1
first full:
Hello
span:
0
..
5
groups count:
1
all:
Hello
@
0
..
5
all:
123
@
6
..
9
all:
world
@
10
..
15

-- More regex demos --
Emails (findall):
one@example.com
Two@Example.COM
URLs:
IPv4:
8.8.8.8
192.168.0.1
Dates (with groups Y/M/D):
1999-12-31
1999/12/31
2025-11-25
2025/11/25
Hex colors:
#FF00FF
#1a2b3c
Quoted strings (with escapes):
"hi there"
"indented"
"quote\"inside"
Multiline ^ anchors (first token of each line):
first
Second
third
Dotall OFF (should fail):
0
Dotall ON (should match):
1
Word boundary, case-insensitive:
The
the
THE
Lookahead (word before number):
foo
baz
Greedy:
<a>one</a><a>two</a>
Non-greedy:
<a>one</a>
<a>two</a>
*/

