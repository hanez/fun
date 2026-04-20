---
layout: page
published: true
noToc: false
noComments: false
noDate: false
title: VM Configuration Constants
subtitle: Detailed explanation of VM limits and memory management constants.
description: Learn about MAX_FRAMES, MAX_FRAME_LOCALS, MAX_GLOBALS, STACK_SIZE, and OUTPUT_SIZE in the Fun VM.
permalink: /documentation/vm/
lang: en
tags:
- coding
- vm
- configuration
---

The Fun Virtual Machine (VM) uses several fixed-size limits to manage memory and execution. These constants are defined in `src/vm.h` and can be adjusted if necessary by re-compiling the project.

This document explains what each of these constants does in plain English.

View [Build](../build/) to see how to set the params at build time.

## `MAX_FRAMES` (Default: 128)

This constant defines the **maximum depth of the call stack**. 

When a function is called, the VM creates a "frame" to keep track of that function's execution (where it's at in the code and its local variables). If a function calls another function, a new frame is added on top. 

- **What happens if you exceed it?** If you have too many nested function calls (for example, in a deep recursion that never ends), the VM will run out of space for new frames.
- **Analogy:** Imagine a stack of dinner plates. `MAX_FRAMES` is the maximum height the stack can reach before it becomes unstable or hits the ceiling.

## `MAX_FRAME_LOCALS` (Default: 64)

This constant limits the **number of local variables** each individual function can have.

Every time a function is called, it gets its own space for variables that only exist within that function. 

- **What happens if you exceed it?** A single function cannot define more than 64 local variables. If it tries to use more, the VM won't be able to store them in the frame.
- **Analogy:** Think of this as the number of pockets in a single person's jacket. You can only carry 64 items in your pockets at once.

## `MAX_GLOBALS` (Default: 128)

This constant defines the **maximum number of global variables** available to the entire program.

Global variables are accessible from anywhere in your code, unlike local variables which belong to a specific function.

- **What happens if you exceed it?** Your program cannot have more than 128 global variables defined at the same time.
- **Analogy:** This is like a shared community bulletin board. There is only enough room on the board for 128 different notices.

## `STACK_SIZE` (Default: 1024)

This constant sets the size of the **operand stack**.

The VM uses this stack for almost everything it does: adding numbers, comparing values, and passing arguments to functions. Most operations take values from the top of the stack, perform a calculation, and push the result back onto the stack.

- **What happens if you exceed it?** Complex calculations or passing a very large number of arguments might fill up this stack, leading to a "stack overflow."
- **Analogy:** Imagine a workbench where you put tools and materials you are currently working on. `STACK_SIZE` is the area of that workbench. If it's too small, you can't work on complex projects.

## `OUTPUT_SIZE` (Default: 1024)

This constant determines the size of the **output buffer**.

When your program uses commands like `PRINT` or `ECHO`, the results are stored in an internal list before they are displayed or processed further. 

- **What happens if you exceed it?** The VM can only keep track of the last 1024 printed items in its internal history.
- **Analogy:** Think of this as a printer's paper tray. It can hold 1024 sheets of printed output. Once it's full, you might need to clear it or it might stop recording new output.
