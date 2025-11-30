# Fun ([https://fun-lang.xyz](https://fun-lang.xyz))

## What is Fun?

Fun is an experiment, just for fun, but Fun works!

Fun is a highly strict programming language, but also highly simple. It looks like Python (My favorite language), but there are differences.

Influenced by **[Bash](https://www.gnu.org/software/bash/)**, **[C](https://en.wikipedia.org/wiki/The_C_Programming_Language)**, **[Lua](https://www.lua.org/)**, PHP, **[Python](https://www.python.org/)**, and Rust (Most influences came from linked languages).

Fun is and will ever be 100% free under the terms of the [Apache-2.0 License](https://opensource.org/license/apache-2-0).

## Idea

- Simplicity
- Consistency
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

### Lib

...

### Extensions

- [CGI](https://en.wikipedia.org/wiki/Common_Gateway_Interface) support builtin using [kcgi](https://kristaps.bsd.lv/kcgi/) (optional) &#9744;
- [cURL](https://curl.se/) support builtin using [libcurl](https://curl.se/libcurl/) (optional) &#9745;
- [INI](https://en.wikipedia.org/wiki/INI_file) support builtin using [iniparser](https://gitlab.com/iniparser/iniparser/) (optional) &#9745;
- [JSON](https://www.json.org/) support builtin using [json-c](https://github.com/json-c/json-c) (optional) &#9745;
- [libSQL](https://github.com/tursodatabase/libsql) support builtin (optional) &#9745;
- [PCRE2](https://pcre2project.github.io/pcre2/) support builtin for Perl-Compatible Regular Expressions (optional) &#9745;
- [PCSC](https://pcscworkgroup.com/) smart card support builtin using [PCSC lite](https://pcsclite.apdu.fr/) (optional) &#9745;
- [SQLite](https://sqlite.org/) support builtin (optional) &#9745;
- [Tk](https://www.tcl-lang.org/) support builtin for GUI application development (optional) &#9744;
- [XML](https://www.w3.org/XML/) support builtin using [libxml2](https://gitlab.gnome.org/GNOME/libxml2/-/wikis/home) (optional) &#9744;

&#9745; = Done / &#9744; = Planned or in progress.

Note: Not all of the above features will be implemented. Those who are marked "Done" will probaly remain in Fun, but I don't know actually... ;)

There are some libs written in Fun available in the [./lib/](https://git.xw3.org/fun/fun/src/branch/main/lib) diretory. In the future most Fun enhancements should be written in Fun itself.

## Documentation

This is actually a work in progress...

Current documentation is only found in the [Fun Handbook](https://git.xw3.org/fun/fun/src/branch/main/docs/handbook.md).

In the [./examples/](https://git.xw3.org/fun/fun/src/branch/main/examples) directory should be an example of most Fun features.

Fun internals are found directly in the [./src/](https://git.xw3.org/fun/fun/src/branch/main/src) diretory. Fun [Opcodes](https://en.wikipedia.org/wiki/Opcode) are found in [./src/vm/](https://git.xw3.org/fun/fun/src/branch/main/src/vm).

Since things are actually changing sometimes, I will not write the documentation for this as of now.

Complete API documentation will follow.

## Author

Johannes Findeisen <you@hanez.org>

