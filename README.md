# Fun ([https://fun-lang.xyz](https://fun-lang.xyz))

## What is Fun?

Fun is a highly strict programming language, but also highly simple. It looks like Python (My favorite language), but there are differences.

Influenced by **[Bash](https://www.gnu.org/software/bash/)**, **[C](https://en.wikipedia.org/wiki/The_C_Programming_Language)**, Go, **[Lua](https://www.lua.org/)**, **[Python](https://www.python.org/)**, and Rust (Most influences came from linked languages).

Fun is and will ever be 100% free under the terms of the [Apache-2.0 License](https://opensource.org/license/apache-2-0).

## Idea

- Simplicity
- Consistency
- Joy in coding
- Fun!

### Extras

- [JSON](https://www.json.org/) support builtin using [json-c](https://github.com/json-c/json-c) (optional) &#9745;
- [ODBC](https://learn.microsoft.com/en-us/sql/odbc/reference/odbc-overview?view=sql-server-ver16) support builtin for flexible database connectivity using [unixODBC](https://www.unixodbc.org/) (optional) &#9744;
- [PC/SC](https://pcscworkgroup.com/) smart card support builtin using [PCSC lite](https://pcsclite.apdu.fr/) (optional) &#9745;
- [SQLite](https://sqlite.org/) support builtin (optional) &#9744;
- [Tk](https://www.tcl-lang.org/) support builtin for GUI application development (optional) &#9744;

&#9745; = Done / &#9744; = Planned or in progress.

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

## Documentation

I am writing documentation only actually, but this is work in progress, since debugging and bug fixing includes this task.

Current documentation is only found in the [Fun Handbook](https://git.xw3.org/fun/fun/src/branch/main/docs/handbook.md).

In the [examples/](https://git.xw3.org/fun/fun/src/branch/main/examples) directory should be an example of most Fun features.

A complete API documentation will follow.

## Development

This section is a work in progress... Please excuse the lack of more information. There are daily updates here.

### Rules

- Every commit message must contain the version at the end in the following format (1.2.3)
- Every commit requires a version incrementation in CMakeLists.txt before committing. Documentation updates do not increment the version but must contain the current version in each commit message.
- Version numbering follows "[Semantic Versioning 2.0.0](https://semver.org/spec/v2.0.0.html)"

### Development systems

- [GNU](https://gnu.org/)/[Linux](https://kernel.org/) ([Arch](https://archlinux.org/)/[Artix](https://artixlinux.org/), [Debian](https://www.debian.org/)) using [GCC](https://gcc.gnu.org/) and the [GNU C library](https://www.gnu.org/software/libc/) ([glibc](https://en.wikipedia.org/wiki/Glibc))
- GNU/Linux ([Alpine](https://alpinelinux.org/)) using GCC and the [musl libc](https://musl.libc.org/)
- [FreeBSD](https://www.freebsd.org/) using [Clang](https://clang.llvm.org/) and the [BSD libc](https://en.wikipedia.org/wiki/C_standard_library#BSD_libc)
- [Windows](https://en.wikipedia.org/wiki/Microsoft_Windows) using [Cygwin](https://www.cygwin.com/) and GCC.

### Other systems

- [macOS](https://en.wikipedia.org/wiki/MacOS), [NetBSD](https://netbsd.org/), [OpenBSD](https://www.openbsd.org/), etc. should fully work, but I don't know. I do not use these systems actually. You wanna try and report?

### To Do

Everything... ;) No, a lot of stuff works already, but only a tiny set of functionality is available in the Fun programming language. It grows from day to day...

### Build Fun

Linux/UNIX only covered here for now.

Clone repository:

```bash
git clone https://git.xw3.org/fun/fun.git
```

Change directory:

```bash
cd fun
```

Build:

```bash
# Note: Every -D flag must be of the form NAME=VALUE (e.g., -DFUN_WITH_REPL=ON)
cmake -S . -B build -DFUN_DEBUG=OFF -DFUN_WITH_PCSC=OFF -DFUN_WITH_REPL=ON -DFUN_WITH_JSON=ON
cmake --build build --target fun
```

CMake options you can toggle (all require NAME=VALUE):

- FUN_DEBUG=ON|OFF — verbose debug logging in the VM (default OFF)
- FUN_WITH_REPL=ON|OFF — enable building the interactive REPL (default ON)
- FUN_WITH_PCSC=ON|OFF — enable PC/SC smart card support (default OFF)
- FUN_WITH_JSON=ON|OFF — enable JSON support via json-c (default OFF)

That's it! For testing it, run:

```bash
FUN_LIB_DIR="$(pwd)/lib" ./build/fun ./demo.fun
```

To see what's going on, run:

```bash
FUN_LIB_DIR="$(pwd)/lib" ./build/fun --trace ./demo.fun
```

To switch into the REPL after an error, run:

```bash
FUN_LIB_DIR="$(pwd)/lib" ./build/fun --repl-on-error --trace ./demo.fun
```

Both --repl-on-error and --trace are optional but can always be combined. To get
more debug information, you need to build Fun with -DFUN_DEBUG=ON.

To directly run the REPL, you have to run:

```bash
FUN_LIB_DIR="$(pwd)/lib" ./build/fun
```

But be sure to build Fun with -DFUN_WITH_REPL=ON.

Tip: If you saw an error like this when configuring with CMake:

  CMake Error: Parse error in command line argument: FUN_WITH_JSON
  Should be: VAR:type=value

it means a -D flag was passed without a value. Always specify options as -DNAME=VALUE, for example:

  -DFUN_WITH_JSON=ON

## Author

Johannes Findeisen <you@hanez.org>

