//!/usr/bin/env fun

/*
 * Debugging and error reporting demo for Fun.
 *
 * How to run:
 *   fun --repl-on-error examples/debug_reporting.fun
 *
 * What happens:
 *   - The program runs compute(), then triggers a runtime error in crash().
 *   - The VM drops you into the REPL at the error site with the stack preserved.
 *
 * Try these REPL commands when it stops:
 *   :backtrace           # or :bt — show frames (most recent first)
 *   :locals              # locals in the current (top) frame
 *   :stack               # show the value stack
 *   :top                 # show the top-of-stack value
 *   :list                # show source around the current line
 *   :disas               # disassemble around current instruction pointer (ip)
 *   :frame 1             # select a lower frame, then try :locals, :list, :disas again
 *   :printv local[0]     # print a single value (also: stack[i], global[i])
 *
 * Stepping and breakpoints:
 *   - Before re-running the script, you can set a breakpoint at the marker below.
 *     For example (adjust the line number as needed in your copy):
 *       :break examples/debug_reporting.fun:LINE_BK_1
 *       :info breaks
 *       :cont
 *     On hit, try stepping:
 *       :step            # step one instruction
 *       :next            # step over
 *       :finish          # run until current frame returns
 *       :cont            # continue until next stop (breakpoint or error)
 *
 * Note: If line numbers drift due to edits, use :list and :backtrace to locate the desired spot,
 * then set the breakpoint with the correct file:line.
 */

/* A small helper computation to set a breakpoint on */
fun add(a, b)
  // Simple arithmetic to have a few instructions to step over
  c = a + b
  print(c)

/* A function that calls add() and has a good place for a breakpoint */
fun compute(n)
  arr = [10, 20, 30, 40, 50]
  i = 2
  // BREAKPOINT CANDIDATE (LINE_BK_1): set a breakpoint on the next line:
  x = arr[i] + 7
  add(x, n)

/* A function that intentionally triggers a runtime error (index out of range) */
fun crash()
  nums = [1, 2, 3]
  // This out-of-range access triggers an error and drops into the REPL:
  print(nums[10])

fun main()
  compute(3)
  crash()

main()

/* Expected when run with --repl-on-error (your version and line/ip may vary):
Runtime error: index out of range
 (at ./examples/debug_reporting.fun:XX in crash, op INDEX_GET @ip YY)
Entering REPL due to runtime error (code 1)
Fun VERSION REPL
Type :help for commands. Submit an empty line to run.
fun> :backtrace
Backtrace (most recent call first):
  #TOP crash at ./examples/debug_reporting.fun ip=... line=...
  #... main at ./examples/debug_reporting.fun ip=... line=...
fun> :list
>    <current line> ...
fun> :locals
  0: [locals if any]
fun> :disas
>      ip: OP_NAME OPERAND
       ...
fun> :stack
[... stack values ...]

Then try breakpoints and stepping on a new run:
fun> :break ./examples/debug_reporting.fun:<line for LINE_BK_1>
fun> :info breaks
fun> :cont
... Breakpoint hit ...
fun> :next
fun> :finish
fun> :cont
*/
