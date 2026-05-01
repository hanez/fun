/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file vm.h
 * @brief Core virtual machine data structures and public VM API.
 *
 * Declares the execution stack/frame layout, global state of the Fun VM,
 * human-readable opcode names for diagnostics, and the public functions for
 * initializing, running, resetting, and debugging the VM. FFI helper
 * declarations for Rust/C++ experiments are also exposed here.
 */
#ifndef FUN_VM_H
#define FUN_VM_H

#include "bytecode.h"
#include <stddef.h>

#ifndef MAX_FRAMES
#define MAX_FRAMES 128
#endif

#ifndef MAX_FRAME_LOCALS
#define MAX_FRAME_LOCALS 64
#endif

#ifndef MAX_GLOBALS
#define MAX_GLOBALS 128
#endif

#ifndef OUTPUT_SIZE
#define OUTPUT_SIZE 1024
#endif

#ifndef STACK_SIZE
#define STACK_SIZE 1024
#endif

static const char *opcode_names[] = {
  "NOP", "LOAD_CONST", "LOAD_LOCAL", "STORE_LOCAL",
  "LOAD_GLOBAL", "STORE_GLOBAL", "ADD", "SUB", "MUL", "DIV",
  "LT", "LTE", "GT", "GTE", "EQ", "NEQ", "POP", "JUMP",
  "JUMP_IF_FALSE", "CALL", "RETURN", "PRINT", "ECHO", "HALT",
  "LINE",
  "MOD", "AND", "OR", "NOT", "DUP", "SWAP",
  "MAKE_ARRAY", "INDEX_GET", "INDEX_SET",
  "LEN", "PUSH", "APOP", "SET", "INSERT", "REMOVE", "SLICE",
  "TO_NUMBER", "TO_STRING", "CAST", "TYPEOF",
  "SPLIT", "JOIN", "SUBSTR", "FIND",
  "REGEX_MATCH", "REGEX_SEARCH", "REGEX_REPLACE",
  "CONTAINS", "INDEX_OF", "CLEAR",
  "ENUMERATE", "ZIP",
  "MIN", "MAX", "CLAMP", "ABS", "POW", "RANDOM_SEED", "RANDOM_INT",
  "MAKE_MAP", "KEYS", "VALUES", "HAS_KEY",
  "READ_FILE", "WRITE_FILE", "ENV", "INPUT_LINE", "PROC_RUN", "PROC_SYSTEM",
  "TIME_NOW_MS", "CLOCK_MONO_MS", "DATE_FORMAT",
  "THREAD_SPAWN", "THREAD_JOIN", "SLEEP_MS",
  "RANDOM_NUMBER",
  "BAND", "BOR", "BXOR", "BNOT", "SHL", "SHR", "ROTL", "ROTR",
  "JSON_PARSE", "JSON_STRINGIFY", "JSON_FROM_FILE", "JSON_TO_FILE",
  "CURL_GET", "CURL_POST", "CURL_DOWNLOAD",
  "SQLITE_OPEN", "SQLITE_CLOSE", "SQLITE_EXEC", "SQLITE_QUERY",
  "LIBSQL_OPEN", "LIBSQL_CLOSE", "LIBSQL_EXEC", "LIBSQL_QUERY",
  "PCSC_ESTABLISH", "PCSC_RELEASE", "PCSC_LIST_READERS", "PCSC_CONNECT", "PCSC_DISCONNECT", "PCSC_TRANSMIT",
  "PCRE2_TEST", "PCRE2_MATCH", "PCRE2_FINDALL",
  "INI_LOAD", "INI_FREE", "INI_GET_STRING", "INI_GET_INT", "INI_GET_DOUBLE", "INI_GET_BOOL", "INI_SET", "INI_UNSET", "INI_SAVE",
  "XML_PARSE", "XML_ROOT", "XML_NAME", "XML_TEXT",
  "SOCK_TCP_LISTEN", "SOCK_TCP_ACCEPT", "SOCK_TCP_CONNECT", "SOCK_SEND", "SOCK_RECV", "SOCK_CLOSE", "SOCK_UNIX_LISTEN", "SOCK_UNIX_CONNECT",
  "FD_SET_NONBLOCK", "FD_POLL_READ", "FD_POLL_WRITE",
  "EXIT",
  "OS_LIST_DIR",
  "SERIAL_OPEN", "SERIAL_CONFIG", "SERIAL_SEND", "SERIAL_RECV", "SERIAL_CLOSE",
  "TRY_PUSH", "TRY_POP", "THROW",
  "FMIN", "FMAX",
  /* Rust FFI demo */
  "RUST_HELLO", "RUST_HELLO_ARGS", "RUST_HELLO_ARGS_RETURN", "RUST_GET_SP", "RUST_SET_EXIT",
  /* C++ demo */
  "CPP_ADD"};

/**
 * @brief Call frame representing one active function invocation.
 *
 * Each frame keeps a pointer to its function bytecode, the current
 * instruction pointer within that bytecode, a fixed-size array of local
 * variables, and a small try/catch stack for exception handling.
 */
typedef struct {
  Bytecode *fn;
  int ip;
  Value locals[MAX_FRAME_LOCALS];
  /* exception handling (per-frame) */
  int try_stack[16];
  int try_sp; /* -1 when empty */
} Frame;

/**
 * @brief The Fun virtual machine state.
 *
 * Holds the operand stack, call frames, globals, standard output capture,
 * runtime counters, and debugger state. Functions in this header operate on
 * this structure; callers must ensure proper initialization with vm_init()
 * before use and call vm_free()/vm_reset() as appropriate.
 */
struct VM {
  Value stack[STACK_SIZE];
  int sp;

  Frame frames[MAX_FRAMES];
  int fp; // frame pointer, -1 when no frame

  Value globals[MAX_GLOBALS];

  Value output[OUTPUT_SIZE]; // store printed values
  int output_count;
  int output_is_partial[OUTPUT_SIZE]; // 1 when the corresponding output entry should not end with newline (echo)

  long long instr_count; // executed instructions in the last vm_run

  int current_line; // last executed source line (debug)

  int exit_code; // process exit code set by OP_EXIT

  int trace_enabled;                   // when non-zero, print executed ops and stack
  int repl_on_error;                   // when non-zero, enter REPL on runtime error (preserve stack)
  int (*on_error_repl)(struct VM *vm); // optional hook to run REPL on error

  /* --- Debugger state --- */
  int debug_step_mode;           // 0 none, 1 step, 2 next, 3 finish
  int debug_step_target_fp;      // target frame pointer for next/finish
  long long debug_step_start_ic; // instruction count snapshot when step/next requested
  int debug_stop_requested;      // force a pause at loop top

  struct {
    char *file; // strdup'ed file path
    int line;   // 1-based line
    int active; // 1 if active
  } breakpoints[64];
  int break_count; // number of active breakpoints
};

/** @brief Opaque VM alias for external users. */
typedef struct VM VM;

/**
 * @brief Initialize a VM instance to zero/initial state.
 * @param vm Non-NULL pointer to VM storage to initialize.
 */
void vm_init(VM *vm);

/**
 * @brief Clear the buffered output captured by the VM.
 * @param vm VM instance.
 */
void vm_clear_output(VM *vm);
/**
 * @brief Print buffered output entries to stdout (debug aid).
 * @param vm VM instance.
 */
void vm_print_output(VM *vm);
/**
 * @brief Free all resources owned by the VM (globals, frames, output buffers).
 * The VM object itself is not freed when allocated on the stack.
 * @param vm VM instance to dispose.
 */
void vm_free(VM *vm);

/**
 * @brief Reset VM to initial state, freeing globals/locals/output.
 * The VM object remains valid for reuse after this call.
 * @param vm VM instance to reset.
 */
void vm_reset(VM *vm);

/**
 * @brief Print non-nil globals (index and value) to stdout.
 * @param vm VM instance.
 */
void vm_dump_globals(VM *vm);
/**
 * @brief Execute the provided entry bytecode in the VM.
 * Pushes an initial frame and runs until HALT or an unrecoverable error.
 * @param vm VM instance.
 * @param entry Entry bytecode to execute; must outlive the call.
 */
void vm_run(VM *vm, Bytecode *entry);

/**
 * @brief Raise a runtime error honoring active try/catch/finally handlers.
 * If a try handler is active in the current frame, control jumps to it with
 * an error string pushed on the stack. Otherwise, prints the error (with
 * location) and terminates execution.
 * @param vm VM instance.
 * @param msg Null-terminated error message.
 */
void vm_raise_error(VM *vm, const char *msg);

/* --- Debugger API --- */
/** Reset debugger state (clear step mode and breakpoints). */
void vm_debug_reset(VM *vm);
/** Add a breakpoint at file:line; returns non-negative id on success or -1. */
int vm_debug_add_breakpoint(VM *vm, const char *file, int line);
/** Delete a breakpoint by id; returns 1 on success, 0 on failure. */
int vm_debug_delete_breakpoint(VM *vm, int id);
/** Remove all breakpoints. */
void vm_debug_clear_breakpoints(VM *vm);
/** Print the current list of breakpoints to stdout. */
void vm_debug_list_breakpoints(VM *vm);
/** Request single-step execution mode. */
void vm_debug_request_step(VM *vm);
/** Step over (next) within the current frame. */
void vm_debug_request_next(VM *vm);
/** Run until the current frame returns (finish). */
void vm_debug_request_finish(VM *vm);
/** Continue execution until next breakpoint/stop. */
void vm_debug_request_continue(VM *vm);

/**
 * @brief Check whether an integer value corresponds to a defined opcode.
 * @param op Numeric opcode to validate.
 * @return 1 if valid, 0 otherwise.
 */
static inline int opcode_is_valid(int op) {
  return op >= OP_NOP && op <= OP_CPP_ADD; // all current opcodes
}

/* --- Minimal C ABI helpers for FFI (Rust opcode experiments) --- */
/** Pop an int64 from the VM stack; accepts int/float; returns truncated value. */
int64_t vm_pop_i64(VM *vm);
/** Push an int64 onto the VM stack. */
void vm_push_i64(VM *vm, int64_t v);

/** Example Rust-implemented opcode (adds top two ints on stack). */
int fun_op_radd(VM *vm);

/** Return a demo null-terminated C string owned by Rust. */
const char *fun_rust_get_string(void);
/** Print a C string via Rust; returns 0 on success. */
int fun_rust_print_string(const char *msg);
/** Return a newly allocated duplicate of the input C string (caller frees). */
char *fun_rust_echo_string(const char *input);
/** Free a C string previously returned by fun_rust_echo_string(). */
void fun_rust_string_free(char *ptr);

/** C++ demo opcode entry point (C ABI). */
int fun_op_cpp_add(struct VM *vm);

/* --- Extended C ABI for Rust to access VM internals (unsafe) --- */
/** Size of struct VM in bytes. */
size_t vm_sizeof(void);
/** Size of struct Value in bytes. */
size_t vm_value_sizeof(void);

/**
 * @brief Get a mutable byte pointer to the VM object.
 * Extremely unsafe; for low-level FFI use only.
 */
void *vm_as_mut_ptr(VM *vm);

/** Offsets of commonly accessed VM fields (for Rust FFI). */
size_t vm_offset_of_exit_code(void);
size_t vm_offset_of_sp(void);
size_t vm_offset_of_stack(void);
size_t vm_offset_of_globals(void);

/* Demo Rust ops using the extended ABI */
int fun_op_rget_sp(VM *vm);
int fun_op_rset_exit(VM *vm);

#endif
