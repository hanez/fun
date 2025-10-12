# Fun Handbook

## Overview

## Introduction

## Installation

### Requirements

A C compiler, a libc and [Git](https://git-scm.com/).

#### FreeBSD

- [CMake](https://cmake.org/)
- [Clang](https://clang.llvm.org/)

#### Linux

- [CMake](https://cmake.org/)
- [GCC](https://gcc.gnu.org/) (Clang should work here too, not tested!)

#### Windows

This requires Cygwin to be installed and configured. I will not cover this here.

- [CMake](https://cmake.org/)
- [Cygwin](https://cygwin.com/) using [GCC](https://gcc.gnu.org/)

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
cmake -S . -B build -DFUN_DEBUG=OFF -DFUN_WITH_PCSC=OFF -DFUN_WITH_REPL=ON &&
cmake --build build --target fun
```

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

### Install Fun to OS

I do not recommend installing Fun on your system because it is in a very early
stage of development, but I can say that I have Fun installed on my system. If
you want to do that too, type:

```bash
sudo cmake --build build --target install
```

Now run Fun without prefixed FUN_LIB_DIR="$(pwd)/lib" because libs are installed to the
system default lib directory (/usr/share/fun/lib).

## Usage

```bash
fun ./demo.fun
```

