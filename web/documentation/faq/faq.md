---
layout: page
published: true
noToc: false
noComments: false
noDate: false
title: Fun - FAQ
subtitle: Frequently asked questions and quick answers.
description: Frequently asked questions and quick answers.
permalink: /documentation/faq/
lang: en
tags:
- answers
- asked
- faq
- frequently
- questions
- quick
---

Answers to common questions.

## I built Fun but includes aren't found

Set `FUN_LIB_DIR` to the repository's `./lib` directory when running without installation:
<pre>FUN_LIB_DIR=./lib ./build/fun examples/hello.fun
</pre>
See [../includes/](../includes/).

## How do I start the REPL?

Run `fun -i` (or run `fun` without a script, depending on version). See [../repl/](../repl/).

## Which build target should I use?

Use the aggregate `build` target to build `fun`, `fun_test`, and `test_opcodes`. See [../build/](../build/).

## Where are the standard libraries?

Under [https://git.xw3.org/fun/fun/src/branch/main/lib](https://git.xw3.org/fun/fun/src/branch/main/lib){:class="git"}. See [../stdlib/](../stdlib/) for an overview.

## Where can I find internals and opcodes?

Browse [https://git.xw3.org/fun/fun/src/branch/main/src/vm](https://git.xw3.org/fun/fun/src/branch/main/src/vm/) and [../internals/](../internals/) / [../opcodes/](../opcodes/).
