---
layout: post
published: true
author: hanez
noToc: false
title: Announcing Fun 0.37.62
description: Homepage and Blog
date: 2026-01-25
date_updated: 2026-01-31 21:45:03
categories:
- news
- project
tags:
- fun
- programming
- static
- dynamic
- language
---

### First at all! Special thanks go to my wonderful girlfriend Sabine, my friends [Armin](https://m2m.pm/){:class="ext"} and [Ole](https://schizzoteam.org/){:class="ext"}, who motivated me over the past month to implement this. Cheers! Markus, I miss YOU, R.I.P!

We're excited to announce the release of Fun version 0.37.62, the latest iteration of our experimental programming language that prioritizes simplicity, consistency, and most importantly — joy in coding.

[Link to Hackernews post](https://news.ycombinator.com/item?id=46787331){:class="ext"}.

### What's Fun All About?

Fun is a highly strict yet highly simple programming language that draws inspiration from Bash, C, Lua, PHP, Python, and Rust. If you love Python's clean syntax but want something with stricter rules and built-in type safety, Fun might be worth exploring. It's dynamically typed with optional static typing, written in C (C99) and Fun itself, and designed to be small, embeddable, and hackable. More on the [about](https://fun-lang.xyz/about/) page.

### Important: This is an Experiment

Please note: Fun is an experimental language and should be treated as such. While Fun works, it has many known and unknown bugs. We will not declare Fun as stable until we reach the 1.0 release. This is a project born out of curiosity and passion for elegant code, not production readiness — at least not yet.

### What Makes Fun... Fun?

#### Philosophy

Fun is built on the idea that coding should be enjoyable, elegant, and consistent. Our manifesto includes core principles like:

- Fun is Fun – Programming should spark creativity, not frustration
- Fun Uses Nothing – Minimalism is power; no unnecessary features or syntax variations
- Indentation is Truth – Two spaces, always (no tabs, no debates, no four-space wars)
- One Way to Do It – No clutter, no 15 ways of writing the same thing. Simplicity means clarity
- Hackable by Nature – Small and embeddable, like Lua. Easy to understand, extend, and tinker with
- Beautiful Defaults – A language that doesn't need linters, formatters, or style guides. Beauty is built in

### What's New in Specification v0.3 (and Fun 0.37.62)

This release brings Fun closer to its vision with significant language enhancements:

- Object-Oriented Programming Classes and objects with methods, constructors, and inheritance using extends
- Explicit this in method definitions for clarity
- Private members by convention (leading underscore)
- Dot-call syntax sugar for clean method invocations: obj.method(a, b)

<pre>
class Counter
  value = 0
  fun inc(this)
    this.value = this.value + 1
    return this.value

c = Counter()
print(c.inc())  // 1
</pre>

### Module System

- Namespaced includes with #include <path> as alias for clean imports
- Organize code into reusable modules without naming conflicts

<pre>
#include <utils/math.fun> as m
print(m.add(2, 3))  // 5
</pre>

### Enhanced Type System

- Fixed-width integers: int8, uint8, int16, uint16, int32, uint32, int64, uint64
- Maps/dictionaries: map<K, V> with literal syntax { "key": value }
- Typed arrays: array<T> for type-safe collections
- Byte type for low-level operations

### Concurrency Support

- Thread primitives: thread_spawn, thread_join, and sleep
- Write concurrent programs with ease

<pre>
fun square(n)
  sleep(100)
  return n * n

tid = thread_spawn(square, 5)
print(thread_join(tid))  // 25
</pre>

### Control Flow Improvements

- Loop control: break and continue statements
- Exception handling: try/catch/finally syntax (runtime implementation evolving)

### System and Network Integration

- Process execution: Blocking (exec, system) and non-blocking (nexec, nspawn) with process management
- TCP and Unix domain sockets: Build networked applications
- Serial port support: Communicate with hardware devices
- Environment variables: Access with env(NAME)
- CLI argument handling: Parse command-line arguments

### Additional Features

- Bitwise operations: band, bor, bxor, bnot, shl
- Rich standard library with modules for cryptography (MD5, SHA1/256/384/512, CRC32), encoding (Base64, hex), JSON, XML, INI, regular expressions (PCRE2), datetime utilities, and more
- Optional extensions for cURL, SQLite/libSQL, PC/SC smart cards, and Tk GUI toolkit

### Core Language Features

Fun provides a clean, readable syntax with:

- Strict indentation-based structure (2 spaces, no semicolons)
- Type safety with no implicit numeric coercions
- Bounds-checked operations to prevent common errors
- Higher-order functions: map, filter, reduce for functional programming
- Multiple return values from functions
- Comprehensive built-ins for common tasks

### The Community: Fun Unites Nerds

Fun isn't about being the fastest or most feature-rich language — it's about sharing the joy of coding. We welcome contributors who are:

- Respectful
- Curious
- Creative

A language that feels like home for developers who love minimal, elegant tools, believe consistency is freedom, and want to write code that looks good and feels good.

### Want to Participate?

Everybody is welcome to participate! However, before you can get started, you need to contact me at [hanez@fun-lang.org](mailto:hanez@fun-lang.org){:class="mail"} to request an account on our Git server.

Important: Fun is not developed on third-party Git servers like GitHub or GitLab. All Fun infrastructure is self-hosted at git.xw3.org. This keeps us independent and aligned with the hacker spirit of the project — we control our tools, our code, and our community.

### Getting Started

If you're curious about Fun, check out:

- [Website](https://fun-lang.xyz)
- [Git Repository](https://git.xw3.org/fun/fun){:class="git"}
- [Fun Handbook](https://fun-lang.xyz/documentation/handbook/){:class="git"}
- [Fun REPL Guide](https://fun-lang.xyz/documentation/repl/){:class="git"}
- [Specification v0.3](https://git.xw3.org/fun/fun/src/branch/main/spec/v0.3.md){:class="git"}
- [Examples](https://git.xw3.org/fun/fun/src/branch/main/examples){:class="git"}
- [Standard Library](https://git.xw3.org/fun/fun/src/branch/main/lib){:class="git"}
- [Community](https://fun-lang.xyz/community/)

The examples directory contains demonstrations of most Fun features, from basic "Hello, World!" to threading, networking, classes, and more. The lib directory includes modules written in Fun itself — and in the future, most enhancements will be written in Fun rather than C.

### For Developers

- [Fun Internals](https://fun-lang.xyz/documentation/internals/){:class="git"}
- [Fun Opcodes](https://fun-lang.xyz/documentation/opcodes/){:class="git"}
- [Basic Rust Opcodes Support](https://fun-lang.xyz/documentation/rust/){:class="git"}

### The Road Ahead

Documentation is a work in progress as we continue debugging, fixing bugs, and evolving the language. Complete API documentation will follow in future releases. Version 0.37.62 represents substantial progress, but remember: this is an experiment, and stability won't be declared until we reach 1.0.

Fun may not change the world, but it aims to make programming a little more fun. We're on a journey, and we'd love to have you along for the ride — bugs and all.

Fun is and will always be 100% free under the terms of the [Apache-2.0 License](https://opensource.org/license/apache-2-0){:class="ext"}.

Happy hacking, and remember: Fun is Fun!

Johannes Findeisen (hanez) - [hanez@fun-lang.org](mailto:hanez@fun-lang.org){:class="mail"}

