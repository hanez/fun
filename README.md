# Fun ([https://fun-lang.xyz](https://fun-lang.xyz))

## What is Fun?

Fun is a small, strict, and simple programming language that runs on a compact stack-based virtual machine. The C core is intentionally minimal; most functionality and standard libraries are implemented in Fun itself. The language emphasizes simplicity, consistency, and joy in coding.

Fun is an experiment, just for fun, but Fun works!

Fun is a highly strict programming language, but also highly simple. It looks like Python (My favorite language), but there are differences.

Influenced by **[Bash](https://www.gnu.org/software/bash/)**, **[C](https://en.wikipedia.org/wiki/The_C_Programming_Language)**, **[Lua](https://www.lua.org/)**, PHP, **[Python](https://www.python.org/)**, and Rust (Most influences came from linked languages).

Fun is and will ever be 100% free under the terms of the [Apache-2.0 License](https://opensource.org/license/apache-2-0).

## Idea

- Simplicity
- Consistency
- Simple to extend
- Hackable
- Joy in coding
- Fun!

## Characteristics

- Dynamic and optionally statically typed
- Type safety
- Written in C (C99) and Fun
- Internal libs are written with no_camel_case even when written in Fun, except class names
- Only a minimal function set is written in C, and most other core functions and libraries are implemented in Fun

## The Fun Manifesto

Fun is a programming language built on a simple idea:
Coding should be enjoyable, elegant, and consistent.

### Philosophy

- **Fun is Fun**<br>
  Programming should spark creativity, not frustration. Code in Fun feels light, playful, and rewarding.
- **Fun Uses Nothing**<br>
  Minimalism is power. No unnecessary features, no endless syntax variations, no formatting debates. Just clean, uniform code.
- **Indentation is Truth**<br>
  Two spaces, always. No tabs, no four-space wars. Code should look the same everywhere, from your laptop to /usr/bin/fun.
- **One Way to Do It**<br>
  No clutter, no 15 ways of writing the same thing. Simplicity means clarity.
- **Hackable by Nature**<br>
  Fun should be small and embeddable, like Lua. Easy to understand, extend, and tinker with — true to the hacker spirit.
- **Beautiful Defaults**<br>
  A language that doesn’t need linters, formatters, or style guides. Beauty is built in.

## The Community

Fun is not about being the fastest or the most feature-rich. It’s about sharing joy in coding. The community should be:

- Respectful
- Curious
- Creative
- Open for everyone

Like the name says: Fun Unites Nerds.

Please visit the [Fun Community Page](https://fun-lang.xyz/community/) to get in touch.

## The Goal

A language that feels like home for developers who:

- Love minimal, elegant tools
- Believe consistency is freedom
- Want to write code that looks good and feels good

Fun may not change the world — but it will make programming a little more fun.

## Features

### Core

- functions/classes/objects
- if/else if/else
- try/catch/finally

### Lib (./lib/)

See [./lib/](https://git.xw3.org/fun/fun/src/branch/main/lib) for what the standard library provides.

### Optional extensions (build-time selectable / only testing this on Linux actually):

- [CGI](https://en.wikipedia.org/wiki/Common_Gateway_Interface) support builtin using [kcgi](https://kristaps.bsd.lv/kcgi/) (optional) &#9744;
- [cURL (libcurl)](./docs/external/curl.md) (optional) &#9745;
- [INI (iniparser)](./docs/external/ini.md) (optional) &#9745;
- [JSON (json-c)](./docs/external/json.md) (optional) &#9745;
- [libSQL](./docs/external/libsql.md) (optional) &#9745;
- [PCRE2](./docs/external/pcre2.md) (optional) &#9745;
- [PCSC (smart cards)](./docs/external/pcsc.md) (optional) &#9745;
- [OpenSSL](./docs/external/openssl.md) (optional) &#9745;
- [SQLite](./docs/external/sqlite.md) (optional) &#9745;
- [Tk (Tcl/Tk GUI)](./docs/external/tcltk.md) (optional) &#9745;
- [XML (libxml2)](./docs/external/xml2.md) (optional) &#9745;

&#9745; = Done / &#9744; = Planned or in progress.

Note: Not all of the above features will be implemented. Those who are marked "Done" will probaly remain in Fun, but I don't know actually... ;)

There are some libs written in Fun available in the [./lib/](https://git.xw3.org/fun/fun/src/branch/main/lib) diretory. In the future most Fun enhancements should be written in Fun itself.

### OpenSSL quickstart (MD5)

See the dedicated page: ./docs/external/openssl.md

## Documentation

Looking for docs? Start here:

- Local documentation index: [docs/README.md](./docs/README.md)
  - Handbook: [docs/handbook.md](./docs/handbook.md)
  - Types overview: [docs/types.md](./docs/types.md)
  - REPL guide: [docs/repl.md](./docs/repl.md)
  - Testing: [docs/testing.md](./docs/testing.md)
  - Troubleshooting: [docs/troubleshooting.md](./docs/troubleshooting.md)

Additional references:

- Specification: [spec/v0.3.md](./spec/v0.3.md) (work in progress)
- Examples demonstrating most features: [examples/](./examples/)
- Internals and VM opcodes live in [src/](./src/) (see [src/vm/](./src/vm) for opcode implementations)

Note: The project is evolving; some documents may lag behind. The docs index in `./docs/README.md` is the most up‑to‑date entry point.

## Author

Johannes Findeisen - <you@hanez.org>
