---
layout: page
published: true
noToc: false
noComments: false
noDate: false
title: Fun - PCRE2 (Perl-Compatible Regex) extension (optional)
subtitle: Documentation for PCRE2 (Perl-Compatible Regex) extension (optional)
description: Documentation for PCRE2 (Perl-Compatible Regex) extension (optional)
permalink: /documentation/external/pcre2/
lang: en
tags:
- compatible
- extension
- optional
- pcre2
- perl
- regex
---


- CMake option: FUN_WITH_PCRE2=ON
- Purpose: Advanced regular expressions via PCRE2.
- Homepage: [https://www.pcre.org/](https://www.pcre.org/){:class="ext"}

## Opcodes:

- OP_PCRE2_TEST: pops flags, text, pattern; pushes 1/0
- OP_PCRE2_MATCH: pops flags, text, pattern; pushes match map or Nil
- OP_PCRE2_FINDALL: pops flags, text, pattern; pushes array of match maps

## Notes:

- Requires PCRE2 development headers/libs.
- Flags are backend-specific; see implementation for supported bits.
