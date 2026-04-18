---
layout: page
published: true
noToc: false
noComments: false
noDate: false
title: Fun - Optional extensions
subtitle: Catalog of optional, build-time selectable extensions for the Fun VM (e.g., cURL, SQLite, JSON).
description: Catalog of optional, build-time selectable extensions for the Fun VM (e.g., cURL, SQLite, JSON).
permalink: /documentation/extensions/
lang: en
tags:
- area
- catalog
- contains
- env
- example
- examples
- extension
- extensions
- extensions
- integrations
- optional
- required
- requirements
- run
- scripts
- them
- under
- vars
- what
---

This section documents Fun's optional, build-time selectable extensions. Each page covers:

- How to enable the extension via CMake option (FUN_WITH_*)
- Build requirements and detection notes
- Available opcodes and/or helper functions
- Minimal usage examples and links to example scripts

## Extensions:

- [cURL (libcurl)](./curl/)
- [INI (iniparser)](./ini/)
- [JSON (json-c)](./json/)
- [libxml2 (XML)](./xml2/)
- [SQLite](./sqlite/)
- [PCRE2 (Perl-compatible regex)](./pcre2/)
- [PC/SC (Smart cards)](./pcsc/)
- [OpenSSL](./openssl/)

## Notes:

- These integrations are optional; the VM compiles without them.
- When disabled, related builtins usually return empty strings/neutral values rather than fail hard, mirroring existing optionality patterns.
