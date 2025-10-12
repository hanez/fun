/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/* Bring in split-out built-ins without changing the build system yet */
#include "iter.c"
#include "map.c"
#include "string.c"
#include "pcsc.c"
#include "vm.h"
#include "value.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifdef __unix__
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/un.h>
#include <unistd.h>
#include <arpa/inet.h>
#endif

/* Threading internals (registry and platform glue) */
#include "vm/os/thread_common.c"

/* Track the currently running VM to annotate error messages */
static VM *g_active_vm = NULL;

/* fprintf wrapper that appends source line info for stderr messages */
static int fun_vm_vfprintf(FILE *stream, const char *fmt, va_list ap) {
    int written = vfprintf(stream, fmt, ap);
    if (stream == stderr && g_active_vm && g_active_vm->current_line > 0) {
        /* Append more context: line number, opcode name, and ip */
        const char *opname = "unknown";
        int ip = -1;
        if (g_active_vm->fp >= 0) {
            Frame *f = &g_active_vm->frames[g_active_vm->fp];
            ip = f->ip - 1;
            if (f->fn && ip >= 0 && ip < f->fn->instr_count) {
                int op = f->fn->instructions[ip].op;
                if (op >= 0 && op < (int)(sizeof(opcode_names)/sizeof(opcode_names[0]))) {
                    opname = opcode_names[op];
                }
            }
        }
        const char *fname = NULL;
        const char *sfile = NULL;
        if (g_active_vm->fp >= 0) {
            Frame *f = &g_active_vm->frames[g_active_vm->fp];
            if (f->fn) {
                fname = f->fn->name;
                sfile = f->fn->source_file;
            }
        }
        fprintf(stream == stderr ? stderr : stream,
                " (at %s:%d in %s, op %s @ip %d)\n",
                sfile ? sfile : "<unknown>",
                g_active_vm->current_line,
                fname ? fname : "<entry>",
                opname,
                ip);
    }
    return written;
}

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

static jmp_buf g_vm_err_jmp;

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

static const char* value_type_name(ValueType t) {
    switch (t) {
        case VAL_FUNCTION: return "function";
        case VAL_INT: return "int";
        case VAL_FLOAT: return "float";
        case VAL_BOOL: return "boolean";
        case VAL_ARRAY: return "array";
        case VAL_MAP: return "map";
        case VAL_NIL: return "nil";
        case VAL_STRING: return "string";
        default: return "unknown";
    }
}

void vm_clear_output(VM *vm) {
    for (int i = 0; i < vm->output_count; ++i) {
        free_value(vm->output[i]);
    }
    vm->output_count = 0;
}

void vm_free(VM *vm) {
    // currently nothing persistent allocated inside VM itself
}

/* forward declaration for helper used in vm_reset */
static void vm_pop_frame(VM *vm);

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

int vm_debug_add_breakpoint(VM *vm, const char *file, int line) {
    if (!file || line <= 0) return -1;
    if (vm->break_count >= (int)(sizeof(vm->breakpoints)/sizeof(vm->breakpoints[0]))) return -1;
    int id = vm->break_count++;
    vm->breakpoints[id].file = strdup(file);
    vm->breakpoints[id].line = line;
    vm->breakpoints[id].active = 1;
    return id;
}

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

void vm_debug_clear_breakpoints(VM *vm) {
    vm_debug_reset(vm);
}

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

void vm_debug_request_step(VM *vm) {
    vm->debug_step_mode = 1; // step
    vm->debug_step_start_ic = vm->instr_count;
    vm->debug_stop_requested = 0;
}

void vm_debug_request_next(VM *vm) {
    vm->debug_step_mode = 2; // next (step over)
    vm->debug_step_target_fp = vm->fp;
    vm->debug_step_start_ic = vm->instr_count;
    vm->debug_stop_requested = 0;
}

void vm_debug_request_finish(VM *vm) {
    vm->debug_step_mode = 3; // finish (until return)
    vm->debug_step_target_fp = vm->fp;
    vm->debug_stop_requested = 0;
}

void vm_debug_request_continue(VM *vm) {
    vm->debug_step_mode = 0;
    vm->debug_stop_requested = 0;
}

static void push_value(VM *vm, Value v) {
    if (vm->sp >= STACK_SIZE - 1) {
        fprintf(stderr, "Runtime error: stack overflow\n");
        exit(1);
    }
    vm->stack[++vm->sp] = v; /* take ownership of v */
}

static Value pop_value(VM *vm) {
    if (vm->sp < 0) {
        fprintf(stderr, "Runtime error: stack underflow\n");
        exit(1);
    }
    return vm->stack[vm->sp--]; /* caller owns returned Value */
}

static void frame_init(Frame *f) {
    f->fn = NULL;
    f->ip = 0;
    for (int i = 0; i < MAX_FRAME_LOCALS; ++i) f->locals[i] = make_nil();
}

void vm_init(VM *vm) {
    vm->sp = -1;
    vm->fp = -1;
    vm->output_count = 0;
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
    for (int i = 0; i < (int)(sizeof(vm->breakpoints)/sizeof(vm->breakpoints[0])); ++i) {
        vm->breakpoints[i].file = NULL;
        vm->breakpoints[i].line = 0;
        vm->breakpoints[i].active = 0;
    }

    for (int i = 0; i < MAX_GLOBALS; ++i)
        vm->globals[i] = make_nil();
}

/* push a new frame, transferring ownership of args[] into frame->locals[0..argc-1] */
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

void vm_print_output(VM *vm) {
    for (int i = 0; i < vm->output_count; ++i) {
        print_value(&vm->output[i]);
        printf("\n");
    }
}

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
            const char *opname = (inst.op >= 0 && inst.op < (int)(sizeof(opcode_names)/sizeof(opcode_names[0])))
                                 ? opcode_names[inst.op] : "???";
            const char *fname = f->fn && f->fn->name ? f->fn->name : "<entry>";
            const char *sfile = f->fn && f->fn->source_file ? f->fn->source_file : "<unknown>";
            /* Dump up to top 4 stack values */
            int count = vm->sp + 1;
            int start = count - 4; if (start < 0) start = 0;
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
            #include "vm/bitwise/bor.c"
            #include "vm/bitwise/bxor.c"
            #include "vm/bitwise/bnot.c"
            #include "vm/bitwise/shl.c"
            #include "vm/bitwise/shr.c"
            #include "vm/bitwise/rol.c"
            #include "vm/bitwise/ror.c"

            #include "vm/core/call.c"
            #include "vm/core/dup.c"
            #include "vm/core/halt.c"
            #include "vm/core/exit.c"
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

            #include "vm/io/read_file.c"
            #include "vm/io/write_file.c"
            #include "vm/io/input_line.c"

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
            #include "vm/math/clamp.c"
            #include "vm/math/max.c"
            #include "vm/math/min.c"
            #include "vm/math/mod.c"
            #include "vm/math/pow.c"
            #include "vm/math/random_int.c"
            #include "vm/math/random_seed.c"

            #include "vm/os/env.c"
            #include "vm/os/sleep_ms.c"
            #include "vm/os/thread_join.c"
            #include "vm/os/thread_spawn.c"
            #include "vm/os/proc_run.c"
            #include "vm/os/proc_system.c"
            #include "vm/os/time_now_ms.c"
            #include "vm/os/clock_mono_ms.c"
            #include "vm/os/date_format.c"
            
            /* Socket ops */
            #include "vm/os/socket_tcp_listen.c"
            #include "vm/os/socket_tcp_accept.c"
            #include "vm/os/socket_tcp_connect.c"
            #include "vm/os/socket_send.c"
            #include "vm/os/socket_recv.c"
            #include "vm/os/socket_close.c"
            #include "vm/os/socket_unix_listen.c"
            #include "vm/os/socket_unix_connect.c"

            #include "vm/pcsc/establish.c"
            #include "vm/pcsc/release.c"
            #include "vm/pcsc/list_readers.c"
            #include "vm/pcsc/connect.c"
            #include "vm/pcsc/disconnect.c"
            #include "vm/pcsc/transmit.c"

            #include "vm/strings/find.c"
            #include "vm/strings/regex_match.c"
            #include "vm/strings/regex_search.c"
            #include "vm/strings/regex_replace.c"
            #include "vm/strings/split.c"
            #include "vm/strings/substr.c"

            #include "vm/len.c"
            #include "vm/line.c"
            #include "vm/print.c"
            #include "vm/to_number.c"
            #include "vm/to_string.c"
            #include "vm/cast.c"
            #include "vm/typeof.c"
            #include "vm/uclamp.c"
            #include "vm/sclamp.c"

            default:
                if (!opcode_is_valid(inst.op)) {
                    fprintf(stderr, "Runtime error: unknown opcode %d (%s) at instruction %d\n",
                            inst.op,
                            (inst.op >= 0 && inst.op < sizeof(opcode_names)/sizeof(opcode_names[0]))
                                ? opcode_names[inst.op] : "???",
                            f->ip - 1);
                    exit(1);
                }
                break;
        }
    }
    g_active_vm = NULL;
}
