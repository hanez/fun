# Standard Library Overview

This page provides a quick orientation to the standard library located under [`./lib/`](../lib/).

The stdlib is written in Fun and organized by domain. Below are top-level modules and what they generally cover. Refer to the source for full APIs and examples.

## Module index (selected)
- `crypt/` - cryptographic helpers (hashing, encoding helpers). See also [lib/crypt](../lib/crypt/).
- `encoding/` - text/binary encodings and conversions.
- `io/` - file and stream utilities.
- `net/` - basic networking helpers.
- `regex/` - regular expression utilities (PCRE2 when available).
- `ui/` - UI helpers (e.g., Tk if enabled at build time).
- `utils/` - small reusable helpers and utilities.

Note: Availability of some modules can depend on optional extensions selected at build time (see [build.md](./build.md)).

## Using modules
```fun
include "utils/strings.fun" as strings

let s = strings.trim("  hello  ")
print(s)
```

For search paths and namespacing details, see [includes.md](./includes.md) and [cli.md](./cli.md) (FUN_LIB_DIR and DEFAULT_LIB_DIR).
