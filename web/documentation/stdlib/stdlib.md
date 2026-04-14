---
layout: page
published: true
noToc: false
noComments: false
noDate: false
title: Fun - Standard Library Overview
subtitle: Overview of the standard library modules under ./lib with one-line summaries.
description: Overview of the standard library modules under ./lib with one-line summaries.
permalink: /documentation/stdlib/
lang: en
tags:
- lib
- library
- line
- modules
- one
- standard
- summaries
- under
---

This page provides a quick orientation to the standard library located under [https://git.xw3.org/fun/fun/src/branch/main/lib](https://git.xw3.org/fun/fun/src/branch/main/lib){:class="git"}.

The stdlib is written in Fun and organized by domain. Below is the current layout with brief notes. Refer to the source for full APIs and examples.

## Top-level modules

- `arrays.fun` — helpers for working with array structures.
- `cli.fun` — minimal helpers for building CLI tools.
- `hello.fun` — simple demonstration helper(s).
- `hex.fun` — hexadecimal encode/decode helpers.
- `math.fun` — math helpers in Fun.
- `regex.fun` — simple regex-related helpers (see also `regex/`).
- `strings.fun` — string manipulation utilities.

## Packages

- `crypt/` — cryptographic primitives and hashes. See [https://git.xw3.org/fun/fun/src/branch/main/lib/crypt/](https://git.xw3.org/fun/fun/src/branch/main/lib/crypt/){:class="git"}.
  - `aes256.fun`
  - `crc32.fun`, `crc32c.fun`
  - `md5.fun`, `md5_legacy.fun`
  - `ripemd160.fun`, `ripemd160_new_try.fun`
  - `sha1.fun`, `sha256.fun`, `sha384.fun`, `sha512.fun`

- `encoding/` — text/binary encodings and conversions. See [https://git.xw3.org/fun/fun/src/branch/main/lib/encoding/](https://git.xw3.org/fun/fun/src/branch/main/lib/encoding/){:class="git"}.
  - `base64.fun`

- `io/` — input/output utilities and system interfaces. See [https://git.xw3.org/fun/fun/src/branch/main/lib/io/](https://git.xw3.org/fun/fun/src/branch/main/lib/io/){:class="git"}.
  - `console.fun` — console I/O helpers
  - `ini.fun` — INI parse helpers
  - `json.fun` — JSON helpers
  - `pcsc.fun`, `pcsc2.fun` — smart card access (PC/SC)
  - `process.fun` — spawn and manage subprocesses
  - `serial.fun` — serial port helpers
  - `socket.fun` — socket convenience wrappers
  - `thread.fun` — simple threading utilities
  - `xml.fun` — XML helpers

- `net/` — networking helpers and example HTTP servers. See [https://git.xw3.org/fun/fun/src/branch/main/lib/net/](https://git.xw3.org/fun/fun/src/branch/main/lib/net/){:class="git"}.
  - `cgi.fun` — basic CGI helpers
  - `http_server.fun` — blocking HTTP server
  - `http_cgi_server.fun` — HTTP server that can execute .fun CGI files
  - `http_cgi_lib_server.fun` — variant of the HTTP CGI server using the stdlib

- `regex/` — regular expression utilities (PCRE2-based when available). See [https://git.xw3.org/fun/fun/src/branch/main/lib/regex/](https://git.xw3.org/fun/fun/src/branch/main/lib/regex/){:class="git"}.
  - `pcre2.fun`

- `utils/` — small reusable helpers and functional utilities. See [https://git.xw3.org/fun/fun/src/branch/main/lib/utils/](https://git.xw3.org/fun/fun/src/branch/main/lib/utils/){:class="git"}.
  - `datetime.fun`
  - `match.fun`
  - `math.fun`
  - `option.fun`
  - `range.fun`
  - `result.fun`

Note: Availability of some modules can depend on optional extensions selected at build time (see [../build/](../build/)). For instance, `regex/pcre2.fun` requires PCRE2 support; `ui/*` depends on chosen UI backends.

## Using modules

<pre>#include &lt;strings.fun

s = trim("  hello  ")
print(s)</pre>

For search paths and namespacing details, see [../includes/](../includes/) and [../cli/](../cli/) (FUN_LIB_DIR and DEFAULT_LIB_DIR).
