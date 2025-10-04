/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

#ifndef FUN_VM_H
#define FUN_VM_H

#include "bytecode.h"

#define MAX_FRAMES 128
#define MAX_FRAME_LOCALS 64
#define MAX_GLOBALS 128
#define OUTPUT_SIZE 1024
#define STACK_SIZE 1024

static const char *opcode_names[] = {
    "NOP","LOAD_CONST","LOAD_LOCAL","STORE_LOCAL",
    "LOAD_GLOBAL","STORE_GLOBAL","ADD","SUB","MUL","DIV",
    "LT","LTE","GT","GTE","EQ","NEQ","POP","JUMP",
    "JUMP_IF_FALSE","CALL","RETURN","PRINT","HALT",
    "LINE",
    "MOD","AND","OR","NOT","DUP","SWAP",
    "MAKE_ARRAY","INDEX_GET","INDEX_SET",
    "LEN","PUSH","APOP","SET","INSERT","REMOVE","SLICE",
    "TO_NUMBER","TO_STRING","CAST","TYPEOF",
    "SPLIT","JOIN","SUBSTR","FIND",
    "REGEX_MATCH","REGEX_SEARCH","REGEX_REPLACE",
    "CONTAINS","INDEX_OF","CLEAR",
    "ENUMERATE","ZIP",
    "MIN","MAX","CLAMP","ABS","POW","RANDOM_SEED","RANDOM_INT",
    "MAKE_MAP","KEYS","VALUES","HAS_KEY",
    "READ_FILE","WRITE_FILE","ENV","INPUT_LINE","PROC_RUN","PROC_SYSTEM",
    "TIME_NOW_MS","CLOCK_MONO_MS","DATE_FORMAT",
    "THREAD_SPAWN","THREAD_JOIN","SLEEP_MS",
    "BAND","BOR","BXOR","BNOT","SHL","SHR","ROTL","ROTR",
    "PCSC_ESTABLISH","PCSC_RELEASE","PCSC_LIST_READERS","PCSC_CONNECT","PCSC_DISCONNECT","PCSC_TRANSMIT",
    "SOCK_TCP_LISTEN","SOCK_TCP_ACCEPT","SOCK_TCP_CONNECT","SOCK_SEND","SOCK_RECV","SOCK_CLOSE","SOCK_UNIX_LISTEN","SOCK_UNIX_CONNECT",
    "EXIT"
};

typedef struct {
    Bytecode *fn;
    int ip;
    Value locals[MAX_FRAME_LOCALS];
} Frame;

typedef struct {
    Value stack[STACK_SIZE];
    int sp;

    Frame frames[MAX_FRAMES];
    int fp; // frame pointer, -1 when no frame

    Value globals[MAX_GLOBALS];

    Value output[OUTPUT_SIZE]; // store printed values
    int output_count;

    long long instr_count; // executed instructions in the last vm_run

    int current_line; // last executed source line (debug)

    int exit_code; // process exit code set by OP_EXIT

    int trace_enabled; // when non-zero, print executed ops and stack
} VM;

// initialize VM (zero state)
void vm_init(VM *vm);

// helper function to clear the output
void vm_clear_output(VM *vm);
void vm_print_output(VM *vm);
void vm_free(VM *vm);

// reset VM to initial state (free globals/locals/output; keep VM object)
void vm_reset(VM *vm);

// print non-nil globals (index and value) to stdout
void vm_dump_globals(VM *vm);
// run entry Bytecode (pushes first frame)
void vm_run(VM *vm, Bytecode *entry);

static inline int opcode_is_valid(int op) {
    return op >= OP_NOP && op <= OP_EXIT;  // all current opcodes
}

#endif
