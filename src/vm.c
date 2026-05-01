/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file vm.c
 * @brief Core virtual machine implementation and opcode dispatch for Fun.
 *
 * Defines the VM state, stack helpers, debugger/stepping support, built-in
 * opcode handlers (included as amalgamated C files), platform I/O helpers,
 * and the main interpreter loop that executes bytecode produced by the Fun
 * compiler. This file is the central runtime of the language.
 */

/* Ensure POSIX/XSI prototypes (nanosleep, wcwidth, etc.) are available
 * before any system headers are included by amalgamated .c files. */
#ifndef _WIN32
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif
#endif

#include <math.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef __unix__
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>
/* For hidden input (password) handling in OP_INPUT_LINE */
#include <termios.h>
/* For non-blocking sockets and polling */
#include <fcntl.h>
#include <poll.h>
// #include <arpa/inet.h>
#endif

#ifdef _WIN32
#include <windows.h>
#endif

/* Bring in split-out built-ins without changing the build system yet */
#include "iter.c"
#include "map.c"
#include "string.c"
#include "value.h"
#include "vm.h"

/* Notcurses support removed */

// Optional by extensions commonly used code. #ifdef's are in each single file.
#include "extensions/curl.c"
#include "extensions/ini.c"
#ifdef FUN_WITH_INI
/* Central INI handle registry and helpers */
#include "vm/ini/handles.c"
#endif
/* Note: INI opcode handlers are included below; changes in vm/ini/ .c files
 * require vm.c to rebuild. */
#include "extensions/json.c"
#include "extensions/openssl.c"
#include "extensions/pcre2.c"
#include "extensions/pcsc.c"
#include "extensions/sqlite.c"
#include "extensions/xml2.c"

/* forward declarations for include mapping used in error reporting */
extern char *preprocess_includes(const char *src);
extern int map_expanded_line_to_include_path(const char *path, int line, char *out_path, size_t out_path_cap, int *out_line);

/* Threading internals (registry and platform glue) */
#include "vm/os/thread_common.c"

/* Track the currently running VM to annotate error messages */
#ifdef _WIN32
static __declspec(thread) VM *g_active_vm = NULL;
#else
static __thread VM *g_active_vm = NULL;
#endif

/**
 * @brief fprintf-like wrapper that annotates stderr messages with VM source context.
 *
 * When writing to stderr and a VM is active, this function appends file name,
 * line number, function name, opcode and instruction pointer information to the
 * message. It attempts to map expanded preprocessed line numbers back to the
 * original included file for clearer diagnostics.
 *
 * @param stream Output stream (typically stderr or stdout).
 * @param fmt printf-style format string.
 * @param ap Variable argument list corresponding to fmt.
 * @return Number of characters written, as returned by vfprintf.
 */
static int fun_vm_vfprintf(FILE *stream, const char *fmt, va_list ap) {
  int written = vfprintf(stream, fmt, ap);
  if (stream == stderr && g_active_vm) {
    /* Determine current frame and instruction pointer of the faulting op */
    const char *opname = "unknown";
    const char *fname = NULL;
    const char *sfile = NULL;
    int ip = -1;
    int line = -1;
    if (g_active_vm->fp >= 0) {
      Frame *f = &g_active_vm->frames[g_active_vm->fp];
      ip = f->ip - 1; /* last executed instruction */
      if (f->fn) {
        fname = f->fn->name;
        sfile = f->fn->source_file;
        /* derive opcode name */
        if (ip >= 0 && ip < f->fn->instr_count) {
          int op = f->fn->instructions[ip].op;
          if (op >= 0 && op < (int)(sizeof(opcode_names) / sizeof(opcode_names[0]))) {
            opname = opcode_names[op];
          }
        }
        /* derive source line by scanning back to the most recent OP_LINE marker */
        for (int i = ip; i >= 0; --i) {
          Instruction prev = f->fn->instructions[i];
          if (prev.op == OP_LINE) {
            line = prev.operand;
            break;
          }
        }
        /* fallback to VM's last recorded line if no marker found */
        if (line <= 0) line = g_active_vm->current_line > 0 ? g_active_vm->current_line : 1;

        /* Map expanded line back to the real included file.
         * Important: OP_LINE operands refer to the expanded, top-level source produced by
         * the preprocessor. Therefore we must pass the TOP-LEVEL script path to the mapper,
         * not the current function's own source_file (which may point at an included file).
         */
        if (line > 0) {
          /* Attempt to retrieve the entry frame's source file as the true top-level path */
          const char *top_path = NULL;
          if (g_active_vm->fp >= 0) {
            Frame *entry = &g_active_vm->frames[0];
            if (entry && entry->fn && entry->fn->source_file) top_path = entry->fn->source_file;
          }
          /* Fallback: use current function's source when entry is unavailable */
          if (!top_path) top_path = sfile;
          if (top_path) {
            char mapped_path[1024];
            int mapped_line = line;
            if (map_expanded_line_to_include_path(top_path, line, mapped_path, sizeof(mapped_path), &mapped_line)) {
              sfile = strdup(mapped_path); /* leak acceptable on error paths */
              line = mapped_line;
            }
          }
        }
      }
    }
    fprintf(stream == stderr ? stderr : stream,
            " (at %s:%d in %s, op %s @ip %d)\n",
            sfile ? sfile : "<unknown>",
            line > 0 ? line : 1,
            fname ? fname : "<entry>",
            opname,
            ip);
  }
  return written;
}

/**
 * @brief fprintf wrapper forwarding to fun_vm_vfprintf.
 *
 * Convenience wrapper that collects varargs and calls fun_vm_vfprintf so that
 * VM-aware diagnostics are consistently applied in this translation unit.
 *
 * @param stream Output stream.
 * @param fmt printf-style format string.
 * @return Number of characters written.
 */
static int fun_vm_fprintf(FILE *stream, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int r = fun_vm_vfprintf(stream, fmt, ap);
  va_end(ap);
  return r;
}


/* Redirect fprintf within this translation unit so opcode handlers use our wrapper */
#define fprintf fun_vm_fprintf

/* Intercept exit() in this translation unit so VM errors don't terminate the process outright */
#include <setjmp.h>

static __thread jmp_buf g_vm_err_jmp;

/**
 * @brief Replacement for exit() used within this translation unit.
 *
 * If REPL-on-error is enabled for the active VM, this performs a longjmp back
 * to vm_run so the REPL can be entered with the current VM state intact.
 * Otherwise, terminates the process immediately using _Exit/_exit.
 *
 * @param code Exit status code (non-zero values indicate error conditions).
 */
static void fun_vm_exit(int code) {
  if (g_active_vm && g_active_vm->repl_on_error) {
    /* Jump back to vm_run to allow dropping into the REPL with intact VM state */
    longjmp(g_vm_err_jmp, code ? code : 1);
  }
  /* Fallback: terminate immediately if not in REPL-on-error mode */
#ifdef _WIN32
  _exit(code);
#else
  _Exit(code);
#endif
}

/* Redirect exit inside this TU (affects included opcode handlers) */
#define exit(code) fun_vm_exit(code)

/* Forward decl for stack push used by vm_raise_error */
static void push_value(VM *vm, Value v);

/* Raise a runtime error that respects try/catch/finally semantics.
 * If a handler is installed for the current frame, jump to it and push
 * an error string for the catch clause. Otherwise, print and stop VM. */
/**
 * @brief Raise a runtime error inside the VM, honoring try/catch/finally.
 *
 * If the current frame has a pending try handler, control is transferred to
 * that handler and the error message is pushed onto the stack for the catch
 * clause. If no handler exists, the error is printed and the VM is stopped.
 *
 * @param vm Pointer to the VM instance.
 * @param msg Human-readable error message (may be NULL).
 */
void vm_raise_error(VM *vm, const char *msg) {
  if (!vm || vm->fp < 0) {
    fprintf(stderr, "Runtime error: %s\n", msg ? msg : "<error>");
    return;
  }
  Frame *f = &vm->frames[vm->fp];
  if (f->try_sp >= 0) {
    char buf[256];
    if (msg) {
      snprintf(buf, sizeof(buf), "Runtime error: %s", msg);
    } else {
      snprintf(buf, sizeof(buf), "Runtime error");
    }
    /* push error value and transfer control to handler target */
    Value err = make_string(buf);
    push_value(vm, err);
    int try_idx = f->try_stack[f->try_sp--];
    int target = f->fn->instructions[try_idx].operand;
    f->ip = target;
    return;
  }
  /* No handler: print annotated message and terminate VM */
  fprintf(stderr, "Runtime error: %s\n", msg ? msg : "<error>");
  vm->fp = -1; /* stop execution */
}

/*
Opcode case include index (vm_case_*.inc):
- Core/stack/frame:
  vm_case_nop.inc, vm_case_halt.inc,
  vm_case_load_const.inc, vm_case_load_local.inc, vm_case_store_local.inc,
  vm_case_load_global.inc, vm_case_store_global.inc,
  vm_case_pop.inc, vm_case_dup.inc, vm_case_swap.inc,
  vm_case_call.inc, vm_case_return.inc, vm_case_print.inc, vm_case_jump.inc, vm_case_jump_if_false.inc
- Arithmetic and logic:
  vm_case_add.inc, vm_case_sub.inc, vm_case_mul.inc, vm_case_div.inc,
  vm_case_mod.inc, vm_case_lt.inc, vm_case_lte.inc, vm_case_gt.inc, vm_case_gte.inc,
  vm_case_eq.inc, vm_case_neq.inc, vm_case_and.inc, vm_case_or.inc, vm_case_not.inc
- Conversions:
  vm_case_to_number.inc, vm_case_to_string.inc
- Arrays and slices:
  vm_case_make_array.inc, vm_case_len.inc,
  vm_case_index_get.inc, vm_case_index_set.inc,
  vm_case_arr_push.inc, vm_case_arr_pop.inc, vm_case_arr_set.inc, vm_case_arr_insert.inc, vm_case_arr_remove.inc,
  vm_case_slice.inc
- Strings and iteration helpers:
  vm_case_split.inc, vm_case_join.inc, vm_case_substr.inc, vm_case_find.inc,
  vm_case_enumerate.inc, vm_case_zip.inc
- Maps and I/O:
  vm_case_make_map.inc, vm_case_keys.inc, vm_case_values.inc, vm_case_has_key.inc,
  vm_case_read_file.inc, vm_case_write_file.inc
- Math utils / RNG:
  vm_case_min.inc, vm_case_max.inc, vm_case_clamp.inc, vm_case_abs.inc, vm_case_pow.inc,
  vm_case_random_seed.inc, vm_case_random_int.inc

Dev tips:
- When adding a new opcode:
  1) Define OP_<NAME> in bytecode.h and opcode_names[] in vm.h.
  2) Implement its VM handler in src/vm_case_<lowercase>.inc.
  3) Include it in the switch below.
  4) Run scripts/check_op_includes.py to verify coverage.
- You can run scripts/run_examples.sh to sanity-check examples quickly.
*/

/**
 * @brief Get a human-readable name for a ValueType.
 *
 * @param t The value type enum.
 * @return Constant string naming the type (e.g., "int", "string").
 */
static const char *value_type_name(ValueType t) {
  switch (t) {
  case VAL_FUNCTION:
    return "function";
  case VAL_INT:
    return "int";
  case VAL_FLOAT:
    return "float";
  case VAL_BOOL:
    return "boolean";
  case VAL_ARRAY:
    return "array";
  case VAL_MAP:
    return "map";
  case VAL_NIL:
    return "nil";
  case VAL_STRING:
    return "string";
  default:
    return "unknown";
  }
}

/**
 * @brief Clear the VM's buffered output values and partial flags.
 *
 * Frees any dynamic storage held by buffered output Values and resets the
 * output counters and partial line indicators.
 *
 * @param vm VM instance whose output buffer should be cleared.
 */
void vm_clear_output(VM *vm) {
  for (int i = 0; i < vm->output_count; ++i) {
    free_value(vm->output[i]);
  }
  vm->output_count = 0;
  // reset partial flags
  for (int i = 0; i < OUTPUT_SIZE; ++i)
    vm->output_is_partial[i] = 0;
}

/**
 * @brief Free resources owned directly by the VM structure.
 *
 * Currently a no-op because the VM does not allocate persistent internal
 * resources outside frames, globals and outputs, which are managed elsewhere.
 *
 * @param vm VM instance to free resources for.
 */
void vm_free(VM *vm) {
  // currently nothing persistent allocated inside VM itself
}

/* forward declaration for helper used in vm_reset */
static void vm_pop_frame(VM *vm);

/**
 * @brief Reset the VM to a clean state.
 *
 * Pops all frames (releasing local variables), clears the operand stack and
 * globals, resets the output buffer and debugger state, and zeros the exit code.
 *
 * @param vm VM instance to reset.
 */
void vm_reset(VM *vm) {
  // Pop all frames (free locals)
  while (vm->fp >= 0) {
    vm_pop_frame(vm);
  }
  // Clear stack
  vm->sp = -1;
  // Free globals
  for (int i = 0; i < MAX_GLOBALS; ++i) {
    free_value(vm->globals[i]);
    vm->globals[i] = make_nil();
  }
  // Clear output buffer
  vm_clear_output(vm);
  // Reset exit code
  vm->exit_code = 0;

  // Reset debugger state (breakpoints, stepping)
  vm_debug_reset(vm);
}

/**
 * @brief Print all non-nil global variables to stdout for debugging.
 *
 * @param vm VM whose globals should be dumped.
 */
void vm_dump_globals(VM *vm) {
  printf("=== globals ===\n");
  for (int i = 0; i < MAX_GLOBALS; ++i) {
    if (vm->globals[i].type != VAL_NIL) {
      printf("[%d] ", i);
      print_value(&vm->globals[i]);
      printf("\n");
    }
  }
  printf("===============\n");
}

/* --- Debugger API impl --- */

/**
 * @brief Reset debugger state: breakpoints and stepping controls.
 *
 * Clears all breakpoints, disables stepping modes, and resets counters.
 *
 * @param vm VM instance with debugger state to reset.
 */
void vm_debug_reset(VM *vm) {
  for (int i = 0; i < vm->break_count; ++i) {
    if (vm->breakpoints[i].file) {
      free(vm->breakpoints[i].file);
      vm->breakpoints[i].file = NULL;
    }
    vm->breakpoints[i].active = 0;
    vm->breakpoints[i].line = 0;
  }
  vm->break_count = 0;
  vm->debug_step_mode = 0;
  vm->debug_step_target_fp = -1;
  vm->debug_step_start_ic = vm->instr_count;
  vm->debug_stop_requested = 0;
}

/**
 * @brief Add a source breakpoint.
 *
 * @param vm VM instance.
 * @param file Source file path (must not be NULL).
 * @param line One-based source line number (> 0).
 * @return Breakpoint id (>=0) on success, -1 on failure (invalid args or full).
 */
int vm_debug_add_breakpoint(VM *vm, const char *file, int line) {
  if (!file || line <= 0) return -1;
  if (vm->break_count >= (int)(sizeof(vm->breakpoints) / sizeof(vm->breakpoints[0]))) return -1;
  int id = vm->break_count++;
  vm->breakpoints[id].file = strdup(file);
  vm->breakpoints[id].line = line;
  vm->breakpoints[id].active = 1;
  return id;
}

/**
 * @brief Delete a breakpoint by id.
 *
 * Compacts the internal breakpoint list to keep ids dense.
 *
 * @param vm VM instance.
 * @param id Breakpoint identifier previously returned by add.
 * @return 1 if deleted, 0 if id was invalid.
 */
int vm_debug_delete_breakpoint(VM *vm, int id) {
  if (id < 0 || id >= vm->break_count) return 0;
  if (vm->breakpoints[id].file) free(vm->breakpoints[id].file);
  for (int i = id + 1; i < vm->break_count; ++i) {
    vm->breakpoints[i - 1] = vm->breakpoints[i];
  }
  vm->break_count--;
  if (vm->break_count >= 0) {
    vm->breakpoints[vm->break_count].file = NULL;
    vm->breakpoints[vm->break_count].line = 0;
    vm->breakpoints[vm->break_count].active = 0;
  }
  return 1;
}

/**
 * @brief Remove all breakpoints from the VM.
 *
 * @param vm VM instance.
 */
void vm_debug_clear_breakpoints(VM *vm) {
  vm_debug_reset(vm);
}

/**
 * @brief Print active breakpoints to stdout.
 *
 * @param vm VM instance.
 */
void vm_debug_list_breakpoints(VM *vm) {
  if (vm->break_count <= 0) {
    printf("(no breakpoints)\n");
    return;
  }
  for (int i = 0; i < vm->break_count; ++i) {
    if (!vm->breakpoints[i].active) continue;
    printf("  [%d] %s:%d\n", i, vm->breakpoints[i].file ? vm->breakpoints[i].file : "<unknown>", vm->breakpoints[i].line);
  }
}

/**
 * @brief Request single-step execution (stop after next instruction).
 *
 * @param vm VM instance.
 */
void vm_debug_request_step(VM *vm) {
  vm->debug_step_mode = 1; // step
  vm->debug_step_start_ic = vm->instr_count;
  vm->debug_stop_requested = 0;
}

/**
 * @brief Request step-over (stop after next instruction in current frame).
 *
 * @param vm VM instance.
 */
void vm_debug_request_next(VM *vm) {
  vm->debug_step_mode = 2; // next (step over)
  vm->debug_step_target_fp = vm->fp;
  vm->debug_step_start_ic = vm->instr_count;
  vm->debug_stop_requested = 0;
}

/**
 * @brief Request finish (run until the current frame returns).
 *
 * @param vm VM instance.
 */
void vm_debug_request_finish(VM *vm) {
  vm->debug_step_mode = 3; // finish (until return)
  vm->debug_step_target_fp = vm->fp;
  vm->debug_stop_requested = 0;
}

/**
 * @brief Resume normal execution (clear stepping state and stop flag).
 *
 * @param vm VM instance.
 */
void vm_debug_request_continue(VM *vm) {
  vm->debug_step_mode = 0;
  vm->debug_stop_requested = 0;
}

/**
 * @brief Push a Value onto the VM operand stack.
 *
 * Takes ownership of the provided Value. Aborts execution on overflow.
 *
 * @param vm VM instance.
 * @param v Value to push (ownership transferred).
 */
static void push_value(VM *vm, Value v) {
  if (vm->sp >= STACK_SIZE - 1) {
    fprintf(stderr, "Runtime error: stack overflow\n");
    exit(1);
  }
  vm->stack[++vm->sp] = v; /* take ownership of v */
}

/**
 * @brief Pop a Value from the VM operand stack.
 *
 * Caller takes ownership of the returned Value. Aborts execution on underflow.
 *
 * @param vm VM instance.
 * @return The top Value from the stack.
 */
static Value pop_value(VM *vm) {
  if (vm->sp < 0) {
    fprintf(stderr, "Runtime error: stack underflow\n");
    exit(1);
  }
  return vm->stack[vm->sp--]; /* caller owns returned Value */
}

/* --- C ABI helpers for Rust FFI --- */
/**
 * @brief Pop a numeric Value and convert it to a 64-bit integer (C ABI helper).
 *
 * Accepts int or float Values on the stack. Other types raise a runtime type
 * error. The popped Value is freed.
 *
 * @param vm VM instance.
 * @return The numeric value converted to int64_t.
 */
int64_t vm_pop_i64(VM *vm) {
  Value v = pop_value(vm);
  int64_t out = 0;
  if (v.type == VAL_INT) {
    out = v.i;
  } else if (v.type == VAL_FLOAT) {
    out = (int64_t)v.d;
  } else {
    fprintf(stderr, "Runtime type error: expected int/float on stack, got %s\n", value_type_name(v.type));
    free_value(v);
    exit(1);
  }
  /* free any dynamic payload (no-op for int/float) */
  free_value(v);
  return out;
}

/**
 * @brief Push a 64-bit integer as a VM int Value (C ABI helper).
 *
 * @param vm VM instance.
 * @param v Integer value to push.
 */
void vm_push_i64(VM *vm, int64_t v) {
  push_value(vm, make_int(v));
}

/* --- Extended C ABI for Rust to access VM internals (unsafe) --- */
/**
 * @brief Return sizeof(VM) for external FFI consumers.
 *
 * @return Size of the VM struct in bytes.
 */
size_t vm_sizeof(void) {
  return sizeof(VM);
}

/**
 * @brief Return sizeof(Value) for external FFI consumers.
 *
 * @return Size of the Value struct in bytes.
 */
size_t vm_value_sizeof(void) {
  return sizeof(Value);
}

/**
 * @brief Cast the VM pointer to an opaque mutable void* (unsafe FFI helper).
 *
 * @param vm VM instance pointer.
 * @return The same pointer reinterpreted as void*.
 */
void *vm_as_mut_ptr(VM *vm) {
  return (void *)vm;
}

/**
 * @brief Obtain offsetof(VM, exit_code) for FFI struct field access.
 *
 * @return Byte offset of the exit_code field within VM.
 */
size_t vm_offset_of_exit_code(void) {
  return offsetof(VM, exit_code);
}

/**
 * @brief Obtain offsetof(VM, sp) for FFI struct field access.
 *
 * @return Byte offset of the sp field within VM.
 */
size_t vm_offset_of_sp(void) {
  return offsetof(VM, sp);
}

/**
 * @brief Obtain offsetof(VM, stack) for FFI struct field access.
 *
 * @return Byte offset of the stack field within VM.
 */
size_t vm_offset_of_stack(void) {
  return offsetof(VM, stack);
}

/**
 * @brief Obtain offsetof(VM, globals) for FFI struct field access.
 *
 * @return Byte offset of the globals field within VM.
 */
size_t vm_offset_of_globals(void) {
  return offsetof(VM, globals);
}

/**
 * @brief Initialize a call frame to a clean state.
 *
 * Sets function pointer and instruction pointer, zeroes locals to nil and
 * resets the try-stack pointer.
 *
 * @param f Frame to initialize.
 */
static void frame_init(Frame *f) {
  f->fn = NULL;
  f->ip = 0;
  for (int i = 0; i < MAX_FRAME_LOCALS; ++i)
    f->locals[i] = make_nil();
  f->try_sp = -1;
}

/**
 * @brief Initialize a VM instance to its default state.
 *
 * Resets stack/frame pointers, output buffers, instruction counters, debugger
 * state and globals. Does not allocate memory.
 *
 * @param vm VM instance to initialize.
 */
void vm_init(VM *vm) {
  vm->sp = -1;
  vm->fp = -1;
  vm->output_count = 0;
  for (int i = 0; i < OUTPUT_SIZE; ++i)
    vm->output_is_partial[i] = 0;
  vm->instr_count = 0;
  vm->exit_code = 0;
  vm->trace_enabled = 0;
  vm->repl_on_error = 0;
  vm->on_error_repl = NULL;

  /* Debugger state */
  vm->debug_step_mode = 0;
  vm->debug_step_target_fp = -1;
  vm->debug_step_start_ic = 0;
  vm->debug_stop_requested = 0;
  vm->break_count = 0;
  for (int i = 0; i < (int)(sizeof(vm->breakpoints) / sizeof(vm->breakpoints[0])); ++i) {
    vm->breakpoints[i].file = NULL;
    vm->breakpoints[i].line = 0;
    vm->breakpoints[i].active = 0;
  }

  for (int i = 0; i < MAX_GLOBALS; ++i)
    vm->globals[i] = make_nil();
}

/* push a new frame, transferring ownership of args[] into frame->locals[0..argc-1] */
/**
 * @brief Push a new call frame for a function and transfer arguments.
 *
 * The first argc Values from args are moved (ownership transfer) into the new
 * frame's local slots starting at index 0. Aborts on frame stack overflow.
 *
 * @param vm VM instance.
 * @param fn Function bytecode to execute in the new frame.
 * @param argc Number of arguments provided.
 * @param args Array of argument Values (may be NULL if argc == 0).
 */
static void vm_push_frame(VM *vm, Bytecode *fn, int argc, Value *args) {
  if (vm->fp >= MAX_FRAMES - 1) {
    fprintf(stderr, "Runtime error: too many frames\n");
    exit(1);
  }
  Frame *f = &vm->frames[++vm->fp];
  frame_init(f);
  f->fn = fn;
  f->ip = 0;
  /* move args into locals 0..argc-1 */
  for (int i = 0; i < argc && i < MAX_FRAME_LOCALS; ++i) {
    f->locals[i] = args[i]; /* transfer ownership */
  }
}

/* pop current frame and free its locals */
/**
 * @brief Pop the current call frame and free its local variables.
 *
 * Aborts if there is no active frame.
 *
 * @param vm VM instance.
 */
static void vm_pop_frame(VM *vm) {
  if (vm->fp < 0) {
    fprintf(stderr, "Runtime error: pop frame with empty frame stack\n");
    exit(1);
  }
  Frame *f = &vm->frames[vm->fp];
  for (int i = 0; i < MAX_FRAME_LOCALS; ++i) {
    free_value(f->locals[i]);
    f->locals[i] = make_nil();
  }
  vm->fp--;
}

/**
 * @brief Print the VM's buffered output values to stdout.
 *
 * Emits a newline after each value unless the corresponding partial flag is set.
 *
 * @param vm VM instance whose output should be printed.
 */
void vm_print_output(VM *vm) {
  for (int i = 0; i < vm->output_count; ++i) {
    print_value(&vm->output[i]);
    if (!vm->output_is_partial[i]) {
      printf("\n");
    }
  }
}

/**
 * @brief Execute a bytecode program starting from the given entry point.
 *
 * Sets up the initial frame and runs the main interpreter loop until there are
 * no more frames. Honors debugger stepping/finish/continue requests and, when
 * enabled, traps exit paths to enter a REPL via on_error_repl.
 *
 * @param vm VM instance to run.
 * @param entry Entry function bytecode (must not be NULL).
 */
void vm_run(VM *vm, Bytecode *entry) {
  /* reset instruction count for this run */
  vm->instr_count = 0;
  vm->current_line = 1;
  g_active_vm = vm;

  /* set error trap if REPL-on-error is enabled */
  if (vm->repl_on_error) {
    int jcode = setjmp(g_vm_err_jmp);
    if (jcode != 0) {
      /* We got here from a trapped exit() in an error path */
      fprintf(stderr, "Entering REPL due to runtime error (code %d)\n", jcode);
      if (vm->on_error_repl) {
        vm->on_error_repl(vm);
      }
      g_active_vm = NULL;
      return;
    }
  }

  /* start with entry frame (no args) */
  vm_push_frame(vm, entry, 0, NULL);

  while (vm->fp >= 0) {
    Frame *f = &vm->frames[vm->fp];

    /* Stop conditions at top of loop (stepping/finish) */
    if (vm->on_error_repl) {
      int should_stop = 0;
      if (vm->debug_stop_requested) {
        should_stop = 1;
      } else if (vm->debug_step_mode == 1 && vm->instr_count > vm->debug_step_start_ic) { /* step */
        should_stop = 1;
        vm->debug_step_mode = 0;
      } else if (vm->debug_step_mode == 2 && vm->instr_count > vm->debug_step_start_ic && vm->fp <= vm->debug_step_target_fp) { /* next */
        should_stop = 1;
        vm->debug_step_mode = 0;
      } else if (vm->debug_step_mode == 3 && vm->fp < vm->debug_step_target_fp) { /* finish */
        should_stop = 1;
        vm->debug_step_mode = 0;
      }
      if (should_stop) {
        vm->debug_stop_requested = 0;
        fprintf(stderr, "Paused (debug)\n");
        vm->on_error_repl(vm);
        /* Frame pointer might have changed (reset/cont); refresh f */
        if (vm->fp < 0) break;
        f = &vm->frames[vm->fp];
      }
    }

    if (f->ip < 0 || f->ip >= f->fn->instr_count) {
      /* no more instructions in this frame -> implicit return nil */
      Value nilv = make_nil();
      vm_pop_frame(vm);
      push_value(vm, nilv);
      continue;
    }

    Instruction inst = f->fn->instructions[f->ip++];
    vm->instr_count++; /* count each executed instruction */

    if (vm->trace_enabled) {
      const char *opname = (inst.op >= 0 && inst.op < (int)(sizeof(opcode_names) / sizeof(opcode_names[0])))
                             ? opcode_names[inst.op]
                             : "???";
      const char *fname = f->fn && f->fn->name ? f->fn->name : "<entry>";
      const char *sfile = f->fn && f->fn->source_file ? f->fn->source_file : "<unknown>";
      /* Dump up to top 4 stack values */
      int count = vm->sp + 1;
      int start = count - 4;
      if (start < 0) start = 0;
      fprintf(stdout, "TRACE %s:%d %s ip=%d %-14s %d | stack[%d]=[", sfile, vm->current_line, fname, f->ip - 1, opname, inst.operand, count);
      for (int i = start; i < count; ++i) {
        char *sv = value_to_string_alloc(&vm->stack[i]);
        if (!sv) sv = strdup("<oom>");
        fprintf(stdout, "%s%s", sv, (i == count - 1 ? "" : ", "));
        free(sv);
      }
      fprintf(stdout, "]\n");
    }

    /* Breakpoint hit detection: breakpoints are set on source_file:line via LINE markers */
    if (vm->on_error_repl && inst.op == OP_LINE && vm->break_count > 0) {
      const char *sfile = (f->fn && f->fn->source_file) ? f->fn->source_file : NULL;
      int line = inst.operand;
      for (int bi = 0; bi < vm->break_count; ++bi) {
        if (!vm->breakpoints[bi].active) continue;
        if (vm->breakpoints[bi].line != line) continue;
        if (!sfile || !vm->breakpoints[bi].file) continue;
        if (strcmp(vm->breakpoints[bi].file, sfile) != 0) continue;
        fprintf(stderr, "Breakpoint %d hit at %s:%d\n", bi, sfile, line);
        vm->on_error_repl(vm);
        /* After returning, refresh frame pointer and frame */
        if (vm->fp < 0) break;
        f = &vm->frames[vm->fp];
        break;
      }
    }

    switch (inst.op) {
/* All opcode handlers as .c includes */
#include "vm/arithmetic/add.c"
#include "vm/arithmetic/div.c"
#include "vm/arithmetic/mul.c"
#include "vm/arithmetic/sub.c"

#include "vm/arrays/apop.c"
#include "vm/arrays/clear.c"
#include "vm/arrays/contains.c"
#include "vm/arrays/enumerate.c"
#include "vm/arrays/index_get.c"
#include "vm/arrays/index_of.c"
#include "vm/arrays/index_set.c"
#include "vm/arrays/insert.c"
#include "vm/arrays/join.c"
#include "vm/arrays/make_array.c"
#include "vm/arrays/push.c"
#include "vm/arrays/remove.c"
#include "vm/arrays/set.c"
#include "vm/arrays/slice.c"
#include "vm/arrays/zip.c"

/* Bitwise and shifts/rotates */
#include "vm/bitwise/band.c"
#include "vm/bitwise/bnot.c"
#include "vm/bitwise/bor.c"
#include "vm/bitwise/bxor.c"
#include "vm/bitwise/rol.c"
#include "vm/bitwise/ror.c"
#include "vm/bitwise/shl.c"
#include "vm/bitwise/shr.c"

#include "vm/core/call.c"
#include "vm/core/dup.c"
#include "vm/core/exit.c"
#include "vm/core/halt.c"
#include "vm/core/jump.c"
#include "vm/core/jump_if_false.c"
#include "vm/core/load_const.c"
#include "vm/core/load_global.c"
#include "vm/core/load_local.c"
#include "vm/core/nop.c"
#include "vm/core/pop.c"
#include "vm/core/return.c"
#include "vm/core/store_global.c"
#include "vm/core/store_local.c"
#include "vm/core/swap.c"
#include "vm/core/throw.c"
#include "vm/core/try_pop.c"
#include "vm/core/try_push.c"

#include "vm/io/input_line.c"
#include "vm/io/read_file.c"
#include "vm/io/write_file.c"

#include "vm/logic/and.c"
#include "vm/logic/eq.c"
#include "vm/logic/gt.c"
#include "vm/logic/gte.c"
#include "vm/logic/lt.c"
#include "vm/logic/lte.c"
#include "vm/logic/neq.c"
#include "vm/logic/not.c"
#include "vm/logic/or.c"

#include "vm/maps/has_key.c"
#include "vm/maps/keys.c"
#include "vm/maps/make_map.c"
#include "vm/maps/values.c"

#include "vm/math/abs.c"
#include "vm/math/ceil.c"
#include "vm/math/clamp.c"
#include "vm/math/cos.c"
#include "vm/math/exp.c"
#include "vm/math/floor.c"
#include "vm/math/fmax.c"
#include "vm/math/fmin.c"
#include "vm/math/gcd.c"
#include "vm/math/isqrt.c"
#include "vm/math/lcm.c"
#include "vm/math/log.c"
#include "vm/math/log10.c"
#include "vm/math/max.c"
#include "vm/math/min.c"
#include "vm/math/mod.c"
#include "vm/math/pow.c"
#include "vm/math/random_int.c"
#include "vm/math/random_seed.c"
#include "vm/math/round.c"
#include "vm/math/sign.c"
#include "vm/math/sin.c"
#include "vm/math/sqrt.c"
#include "vm/math/tan.c"
#include "vm/math/trunc.c"

/* Rust FFI demo opcode(s) */
#include "vm/rust/get_sp.c"
#include "vm/rust/hello.c"
#include "vm/rust/hello_args.c"
#include "vm/rust/hello_args_return.c"
#include "vm/rust/set_exit.c"

#include "vm/os/clock_mono_ms.c"
#include "vm/os/date_format.c"
#include "vm/os/env.c"
#include "vm/os/env_all.c"
#include "vm/os/fun_version.c"
#include "vm/os/proc_run.c"
#include "vm/os/proc_system.c"
#include "vm/os/random_number.c"
#include "vm/os/serial_close.c"
#include "vm/os/serial_config.c"
#include "vm/os/serial_open.c"
#include "vm/os/serial_recv.c"
#include "vm/os/serial_send.c"
#include "vm/os/sleep_ms.c"
#include "vm/os/thread_join.c"
#include "vm/os/thread_spawn.c"
#include "vm/os/time_now_ms.c"

/* Socket ops */
#include "vm/os/socket_close.c"
#include "vm/os/socket_recv.c"
#include "vm/os/socket_send.c"
#include "vm/os/socket_tcp_accept.c"
#include "vm/os/socket_tcp_connect.c"
#include "vm/os/socket_tcp_listen.c"
#include "vm/os/socket_unix_connect.c"
#include "vm/os/socket_unix_listen.c"

/* Async-friendly FD helpers (UNIX) */
#include "vm/os/fd_set_nonblock.c"
#include "vm/os/fd_poll_read.c"
#include "vm/os/fd_poll_write.c"

#ifdef FUN_WITH_PCSC
#include "vm/pcsc/connect.c"
#include "vm/pcsc/disconnect.c"
#include "vm/pcsc/establish.c"
#include "vm/pcsc/list_readers.c"
#include "vm/pcsc/release.c"
#include "vm/pcsc/transmit.c"
#endif

/* JSON ops (implemented in jsonc.c, included above) */
#ifdef FUN_WITH_JSON
#include "vm/json/from_file.c"
#include "vm/json/parse.c"
#include "vm/json/stringify.c"
#include "vm/json/to_file.c"
#endif

/* XML ops (libxml2) */
#ifdef FUN_WITH_XML2
#include "vm/xml/name.c"
#include "vm/xml/parse.c"
#include "vm/xml/root.c"
#include "vm/xml/text.c"
#endif

/* INI ops (iniparser 4.2.6) */
#ifdef FUN_WITH_INI
#include "vm/ini/free.c"
#include "vm/ini/get_bool.c"
#include "vm/ini/get_double.c"
#include "vm/ini/get_int.c"
#include "vm/ini/get_string.c"
#include "vm/ini/load.c"
#include "vm/ini/save.c"
#include "vm/ini/set.c"
#include "vm/ini/unset.c"
#else
#include "vm/ini/stubs.c"
#endif

/* CURL ops */
#ifdef FUN_WITH_CURL
#include "vm/curl/download.c"
#include "vm/curl/get.c"
#include "vm/curl/post.c"
#endif

/* OpenSSL ops (md5/sha256/sha512/ripemd160) */
#ifdef FUN_WITH_OPENSSL
#include "vm/openssl/md5.c"
#include "vm/openssl/ripemd160.c"
#include "vm/openssl/sha256.c"
#include "vm/openssl/sha512.c"
#endif


/* Tcl/Tk support removed */

/* Notcurses TUI ops removed */

/* SQLite ops */
#ifdef FUN_WITH_SQLITE
#include "vm/sqlite/close.c"
#include "vm/sqlite/exec.c"
#include "vm/sqlite/open.c"
#include "vm/sqlite/query.c"
#endif

/* C++ demo opcodes (guarded) */
#if defined(FUN_WITH_CPP)
    case OP_CPP_ADD: {
      int rc = fun_op_cpp_add(vm);
      if (rc != 0) {
        vm_raise_error(vm, "cpp_add failed");
      }
      break;
    }
#else
    case OP_CPP_ADD: {
      vm_raise_error(vm, "CPP support is not enabled (build with -DFUN_WITH_CPP=ON)");
      break;
    }
#endif

/* libsql support removed */

/* PCRE2 ops */
#ifdef FUN_WITH_PCRE2
#include "vm/pcre2/findall.c"
#include "vm/pcre2/match.c"
#include "vm/pcre2/test.c"
#endif

#include "vm/strings/find.c"
#include "vm/strings/regex_match.c"
#include "vm/strings/regex_replace.c"
#include "vm/strings/regex_search.c"
#include "vm/strings/split.c"
#include "vm/strings/substr.c"

#include "vm/cast.c"
#include "vm/echo.c"
#include "vm/len.c"
#include "vm/line.c"
#include "vm/os/list_dir.c"
#include "vm/print.c"
#include "vm/sclamp.c"
#include "vm/to_number.c"
#include "vm/to_string.c"
#include "vm/typeof.c"
#include "vm/uclamp.c"

    default:
      if (!opcode_is_valid(inst.op)) {
        fprintf(stderr, "Runtime error: unknown opcode %d (%s) at instruction %d\n",
                inst.op,
                (inst.op >= 0 && inst.op < sizeof(opcode_names) / sizeof(opcode_names[0]))
                  ? opcode_names[inst.op]
                  : "???",
                f->ip - 1);
        exit(1);
      }
      break;
    }

    /* Stream console output in realtime for scripts:
     * When PRINT/ECHO pushed items into the VM's output buffer, flush them
     * immediately to stdout and clear the buffer to avoid end-of-run bursts.
     * This keeps REPL compatibility (REPL still prints after each submit),
     * while regular script execution shows progress bars live.
     */
    if (inst.op == OP_PRINT || inst.op == OP_ECHO) {
      vm_print_output(vm);
      vm_clear_output(vm);
      fflush(stdout);
    }
  }
  g_active_vm = NULL;
}
