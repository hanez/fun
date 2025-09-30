# Fun

## What is Fun?

Fun is a highly strict programming language, but also highly simple. It looks like Python (My favorite language), but there are differences.

Influenced by Bash, **[C](https://en.wikipedia.org/wiki/The_C_Programming_Language)**, Go, **[Lua](https://www.lua.org/)**, **[Python](https://www.python.org/)**, and Rust (Most influences came from linked languages).

Fun is and will ever be 100% free under the terms of the [ISC-License](https://opensource.org/license/isc-license-txt).

## Idea

* Simplicity
* Consistency
* Joy in coding
* Fun!

## Characteristics

* Dynamic and optionally statically typed
* Type safety
* Written in C
* Internal libs are written with no_camel_case even when written in Fun
* Only a minimal function set is written in C and most other core libraries are implemented in Fun

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

## The Goal

A language that feels like home for developers who:

- Love minimal, elegant tools
- Believe consistency is freedom
- Want to write code that looks good and feels good

Fun may not change the world — but it will make programming a little more fun.

## Documentation

Actually there does not exist any documenation. In the [examples/](https://git.xw3.org/fun/fun/src/branch/main/examples){:class="git"} directory should be an example of most Fun features.

## Development

This section is a work in progress... Please excuse the lack of more information. There are daily updates here.

### Requirements

A C compiler, a libc and [Git](https://git-scm.com/).

Actually Fun requires both CMake and Make as build utils. You always can build Fun only using CMake. Make is just used as a wrapper around CMake.

#### FreeBSD

- [CMake](https://cmake.org/)
- [Clang](https://clang.llvm.org/)
- [GNU Make](https://www.gnu.org/software/make) (gmake)

#### Linux

- [CMake](https://cmake.org/)
- [GCC](https://gcc.gnu.org/) (Clang should work here too, not tested!)
- [GNU make](https://www.gnu.org/software/make) (make)

#### Windows

This requires Cygwin to be installed and configured. I will not cover this here.

- [CMake](https://cmake.org/)
- [GNU make](https://www.gnu.org/software/make) (make)
- [MinGW](https://en.wikipedia.org/wiki/MinGW)

### Rules

- Every commit requires a version incremantion in CMakeLists.txt before committing
- Every commit message must contain the version at the end in the following format (1.2.3)
- Version numbering follows "[Semantic Versioning 2.0.0](https://semver.org/spec/v2.0.0.html)"

### Developement systems

- [GNU](https://gnu.org/)/[Linux](https://kernel.org/) ([Arch](https://archlinux.org/)/[Artix](https://artixlinux.org/), [Debian](https://www.debian.org/)) using [GCC](https://gcc.gnu.org/) and the [GNU C library](https://www.gnu.org/software/libc/) ([glibc](https://en.wikipedia.org/wiki/Glibc))
- GNU/Linux ([Alpine](https://alpinelinux.org/)) using GCC and the [musl libc](https://musl.libc.org/)
- [FreeBSD](https://www.freebsd.org/) using [Clang](https://clang.llvm.org/) and the [BSD libc](https://en.wikipedia.org/wiki/C_standard_library#BSD_libc)
- [Windows](https://en.wikipedia.org/wiki/Microsoft_Windows) using [MinGW](https://en.wikipedia.org/wiki/MinGW)

### Other systems

- [macOS](https://en.wikipedia.org/wiki/MacOS), [NetBSD](https://netbsd.org/), [OpenBSD](https://www.openbsd.org/), etc. should fully work, but I don't know. I do not use these systems actually. You wanna try and report?

### To Do

Everything... ;) No, a lot of stuff works already, but only a tiny set of functionality is available in the Fun programming language. It grows from day to day...

### Build Fun

Linux/UNIX only covered here for now.

Clone repository:

```
git clone https://git.xw3.org/fun/fun.git
```

Change directory:

```
cd fun
```

Build:

```
make # gmake on FeeBSD
```

That's it! For testing it run:

```
./build/fun ./demo.fun
```
