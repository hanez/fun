---
layout: page
published: true
noToc: false
noComments: false
noDate: false
title: REPL Guide
subtitle: REPL guide, how to build/launch, editing and history, completions, REPL-on-error, and tips.
description: REPL guide, how to build/launch, editing and history, completions, REPL-on-error, and tips.
permalink: /documentation/repl/
lang: en
tags:
- build
- completions
- editing
- error
- history
- launch
- tips
- repl
- cli
- fun_with_repl
---

This document describes the interactive Read–Eval–Print Loop (REPL) for the Fun programming language: how to build/launch it, how input and execution work, line editing and completion, the REPL buffer workflow, commands, debugging helpers, and tips.

The REPL is optional at build time. It provides a fast feedback loop for experimenting with Fun code, inspecting VM state, and debugging scripts interactively.

## Enable and build

- Build flag: -DFUN_WITH_REPL=ON
- Typical CMake configuration example:

<pre>cmake -S . -B build \
  -DFUN_WITH_REPL=ON
cmake --build build --target fun</pre>

You can also set a default search path for the bundled stdlib using DEFAULT_LIB_DIR at configure time (used for completions and library loading):

<pre>cmake -S . -B build -DFUN_WITH_REPL=ON -DDEFAULT_LIB_DIR="/usr/share/fun/lib"</pre>

## Launching the REPL

- Directly run the main executable (ensure FUN_WITH_REPL=ON):

<pre>FUN_LIB_DIR="$(pwd)/lib" ./build/fun</pre>

- With the CMake “repl” convenience target (available only if built with FUN_WITH_REPL=ON):

<pre>cmake --build build --target repl</pre>

On startup, you should see something like:

<pre>Fun X.Y.Z REPL
Type :help for commands. Submit an empty line to run.</pre>

Environment variable FUN_LIB_DIR can be used to point the REPL to the standard library directory for symbol completion and library loading. If not set, a compile-time DEFAULT_LIB_DIR (if provided) or "lib" is used.

## Running scripts and REPL-on-error

- Run a script file normally:

<pre>FUN_LIB_DIR="$(pwd)/lib" ./build/fun ./demo.fun</pre>

- Enable tracing, and drop into a REPL automatically when a runtime error occurs:

<pre>FUN_LIB_DIR="$(pwd)/lib" ./build/fun --repl-on-error --trace ./demo.fun</pre>

Inside REPL-on-error, you can inspect frames, locals, disassembly, set breakpoints, and continue or step. Use :help to see available commands.

## Prompts and input model

- Primary prompt: `fun> ` when the input buffer is empty.
- Continuation prompt:
  - `... ` when text exists but no extra indentation is required.
  - `...N> ` (e.g., `...1> `, `...2> `) when the REPL detects open blocks needing additional indentation (2 spaces per level). This helps write multi-line constructs.
- Execution trigger: submit an empty line (press Enter on a blank line). The REPL parses the current buffer and executes it if complete; otherwise it will tell you the input looks incomplete and keep the buffer.

Notes on completeness detection:

- The REPL considers code incomplete if there are unbalanced blocks or the last significant line ends with an operator/comma.
- If incomplete, it prints a hint like “(incomplete, open block indent +N)” or “(incomplete, continue typing)”.

## Line editing and history

The REPL includes an integrated line editor (on UNIX-like systems) with support for:

- Left/Right arrows: move cursor (single-line mode). In multi-line input the cursor stays at end for simplicity.
- Up/Down arrows: navigate input history. Your current line is saved/restored while browsing.
- Ctrl+Left / Ctrl+Right: jump by words.
- Backspace/Delete: erase characters.
- Ctrl+O: insert a newline at the cursor (quick multi-line editing).
- Enter: accept line (adds a newline to the buffer; an empty line executes the buffer).

History is persisted in a file named .fun_history in your home directory (HOME/USERPROFILE) when available, otherwise in the current directory.

## Tab completion

Tab provides two kinds of completions:

- :load path completion
   - When typing a :load command, Tab completes filesystem paths, including directories. A trailing slash is handled as expected. Completion attempts to compute common suffixes across candidates.

- Standard library identifier completion
   - For general input, Tab attempts to complete identifiers from the standard library symbols scanned from FUN_LIB_DIR (or DEFAULT_LIB_DIR/lib). If there are multiple matches, a menu of candidates is printed; otherwise the identifier is completed in place.

If Tab is pressed in the middle of the line (not at end), the REPL beeps instead of completing.

## Buffer workflow

You typically build code incrementally:

- Type lines; they accumulate in an internal buffer.
- Press Enter on a blank line to parse and execute the buffer.
- Output is printed and the buffer is cleared.

Alternatively, use the :run command to execute the buffer immediately (without needing a blank line), or :run <file> to execute a file’s contents.

## Timing and profiling

- Toggle a simple elapsed time measurement for executions with :time on/off/toggle.
- Use :profile to parse+run and report parse time, run time, total, and instruction count.

## Command reference

Type :help to print the built-in command summary. Full list with clarifications:

- :help | :h<br>
  Show the help.

- :quit | :q | :exit<br>
  Exit the REPL.

- :reset | :re<br>
  Reset VM state (clears globals).

- :dump | :du | :globals | :gl<br>
  Dump current globals (indexes and stringified values).

- :globals [pattern] / :vars | :v [pattern]<br>
  Dump globals, filtering by substring match on the value when a pattern is provided.

- :clear | :cl<br>
  Clear the current input buffer.

- :print | :pr<br>
  Show the current buffer content.

- :run | :ru [file]<br>
  Execute current buffer, or execute the specified file immediately. Parsing errors are reported with caret highlighting.

- :profile | :pf<br>
  Execute buffer and show timing for parse and run plus instruction count.

- :save | :sa <file><br>
  Save the current buffer to a file.

- :load | :lo <file><br>
  Load a file into the buffer (does not run). Use :run or a blank line to execute afterward.

- :paste | :pa [run]<br>
  Enter paste mode to insert multiple lines verbatim. Finish with a single dot line: `.`. If the optional argument `run` (or `exec`) is given, the REPL will run the pasted buffer immediately.

- :history | :hi [N]<br>
  Show the last N lines of persistent history (default 50).

- :time | :ti on|off|toggle<br>
  Toggle/enable/disable timing for subsequent runs.

- :env | :en [NAME[=VALUE]]<br>
  Get or set an environment variable. With NAME only, prints NAME=value. With NAME=VALUE, sets the variable for the current process.

- :backtrace | :bt | :ba<br>
  Show a backtrace of VM frames (most recent first), including function name, source file, IP, and line.

- :frame | :fr N<br>
  Select a frame N (0..top) to target with :locals, :list, :disasm and value inspections. By default, the top frame is used.

- :list | :li [±K]<br>
  Show K lines of source around the current frame’s line (default 5). The current line is marked with `>`.

- :disasm | :di [±N]<br>
  Disassemble around current frame’s instruction pointer (default 5 on each side). Shows index, opcode name, and operand.

- :mdump | :md WHAT [offset [len]] [raw] [to &lt;file&gt;]<br>
  Dump a VM memory region. WHAT is one of: code, stack, globals and consts. Offset and length are optional; if omitted, a sensible default (up to 256 bytes) is used. With `raw`, write binary bytes. With `to <file>`, write output to a file; otherwise print to stdout as a formatted hexdump.

- :stack | :st [N]<br>
  Show top N (or all) stack values, stringified.

- :top | :to<br>
  Show the value at the top of the VM stack.

- :locals | :lc [FRAME]<br>
  Show non-nil locals for the selected frame (or the provided frame index).

- :printv | :pv WHAT<br>
  Print a specific value: `local[i]`, `stack[i]`, or `global[i]`.

- :break | :br [file:]line<br>
  Set a breakpoint. If file is omitted, the current frame’s source file is used. Prints a numeric breakpoint ID on success.

- :info | :in breaks<br>
  List breakpoints.

- :delete | :de ID<br>
  Delete a breakpoint by ID.

- :clear breaks | :cb<br>
  Remove all breakpoints.

- :cont | :co<br>
  Continue execution. In REPL-on-error/debug stops, this exits the REPL and resumes the program.

- :step | :sp<br>
  Step a single instruction (REPL-on-error/debug mode).

- :next | :ne<br>
  Step over in the current frame (REPL-on-error/debug mode).

- :finish | :fi<br>
  Run until the current frame returns (REPL-on-error/debug mode).

If an unknown command is entered, the REPL prints a hint to use :help.

## Output handling

Program output produced by VM execution is collected and then printed after each run. After printing, the VM’s output buffer is cleared.

## Errors and diagnostics

- Parse errors are reported with file:line and a caret pointing to the column. When tracing is enabled during normal execution, the VM annotates output with file/line and function names to aid debugging.
- In FUN_DEBUG builds, parse errors are also appended as comments to the history file to assist in later review.

## Environment and library path

- FUN_LIB_DIR: Overrides the standard library search path and the directory scanned for identifier completion.
- DEFAULT_LIB_DIR: Compile-time fallback path used when FUN_LIB_DIR is not set. If neither is available, the REPL defaults to the relative path "lib".

## Tips

- Use :paste run to quickly paste and execute multi-line code from the clipboard.
- :profile is handy to compare parser vs. VM time and to observe instruction counts for larger snippets.
- Combine --repl-on-error with --trace when running scripts to drop into an interactive diagnostic session at the point of failure.
- Use :mdump code/consts/stack/globals to inspect raw VM data, and :disasm to view bytecode in a human-readable form.

## See also

- [Handbook](../handbook/) — full language and VM handbook, including build options and ecosystem overview.
- [https://git.xw3.org/fun/fun/src/branch/main/examples/error/repl_on_error.fun](https://git.xw3.org/fun/fun/src/branch/main/examples/error/repl_on_error.fun){:class="git"} — example showing the REPL-on-error workflow.
