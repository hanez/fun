/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

#ifndef FUN_BYTECODE_H
#define FUN_BYTECODE_H

#include <stdint.h>
#include "value.h"

// VM opcodes
typedef enum {
    OP_NOP,
    OP_LOAD_CONST,     // operand = constant index
    OP_LOAD_LOCAL,     // operand = local slot index
    OP_STORE_LOCAL,    // operand = local slot index

    OP_LOAD_GLOBAL,    //
    OP_STORE_GLOBAL,   //

    OP_ADD,   //
    OP_SUB,   //
    OP_MUL,   //
    OP_DIV,   //

    OP_LT,       // a < b -> push 1/0
    OP_LTE,      // a <= b -> push 1/0
    OP_GT,       // a > b -> push 1/0
    OP_GTE,      // a >= b -> push 1/0
    OP_EQ,       // a == b -> push 1/0
    OP_NEQ,      // a != b -> push 1/0

    OP_POP,            // dApache-2.0ard top of stack
    OP_JUMP,           // unconditional jump
    OP_JUMP_IF_FALSE,  // jump if top of stack is false (0)

    OP_CALL,           // operand = arg count; pops fn + args and enters fn
    OP_RETURN,         // pop optional return value and return to caller

    OP_PRINT,
    OP_HALT,

    OP_LINE,         // operand = source line number (debug marker)

    // add after existing opcodes
    OP_MOD,          // a % b
    OP_AND,      // logical AND
    OP_OR,       // logical OR
    OP_NOT,      // logical NOT

    OP_DUP,          // duplicate top of stack
    OP_SWAP,         // swap top two stack values

    // arrays
    OP_MAKE_ARRAY,    // operand = element count; pops N values, pushes array
    OP_INDEX_GET,     // pops index, array; pushes element copy
    OP_INDEX_SET,     // pops value, index, array; sets in place

    // array and builtin helpers
    OP_LEN,           // pops array or string; pushes length
    OP_PUSH,      // pops value, array; pushes new length
    OP_APOP,       // pops array; pushes removed element
    OP_SET,       // pops value, index, array; pushes value
    OP_INSERT,    // pops value, index, array; pushes new length
    OP_REMOVE,    // pops index, array; pushes removed element
    OP_SLICE,         // pops end, start, array; pushes new array

    // conversions
    OP_TO_NUMBER,     // pops any; pushes int (parse strings)
    OP_TO_STRING,     // pops any; pushes string
    OP_CAST,          // pops typeName, value; pushes casted value (see vm/cast.c)
    OP_TYPEOF,        // pops any; pushes string name of type
    OP_UCLAMP,        // pops number; pushes number masked to N bits (operand = bits)
    OP_SCLAMP,        // pops number; pushes number clamped to signed N-bit range (operand = bits)

    // string ops
    OP_SPLIT,         // pops sep, string; pushes array of strings
    OP_JOIN,          // pops sep, array; pushes string
    OP_SUBSTR,        // pops len, start, string; pushes string
    OP_FIND,          // pops needle, haystack; pushes int index or -1

    // regex ops (POSIX)
    OP_REGEX_MATCH,   // pops pattern, string; pushes 1/0 for full match
    OP_REGEX_SEARCH,  // pops pattern, string; pushes map {"match":str, "start":int, "end":int, "groups":array}
    OP_REGEX_REPLACE, // pops repl, pattern, string; pushes string with global replacements

    // array utils
    OP_CONTAINS,      // pops value, array; pushes 1/0
    OP_INDEX_OF,      // pops value, array; pushes index or -1
    OP_CLEAR,         // pops array; clears it; pushes nothing (we'll push 0)

    // iteration helpers
    OP_ENUMERATE,     // pops array; pushes array of [index, value]
    OP_ZIP,           // pops b, a; pushes array of [a[i], b[i]]

    // math
    OP_MIN,           // pops b, a; pushes min(a,b)
    OP_MAX,           // pops b, a; pushes max(a,b)
    OP_CLAMP,         // pops hi, lo, x; pushes clamped
    OP_ABS,           // pops x; pushes |x|
    OP_POW,           // pops b, a; pushes a^b
    OP_RANDOM_SEED,   // pops seed; sets RNG seed; pushes nothing (we'll push 0)
    OP_RANDOM_INT,    // pops hi, lo; pushes random int in [lo, hi)

    // maps
    OP_MAKE_MAP,      // operand = pair count; pops 2*n (key,value)..., pushes map
    OP_KEYS,          // pops map; pushes array of keys
    OP_VALUES,        // pops map; pushes array of values
    OP_HAS_KEY,       // pops key, map; pushes 1/0

    // file I/O
    OP_READ_FILE,     // pops path string; pushes content string (or "")
    OP_WRITE_FILE,    // pops data string, path string; pushes 1/0

    // OS
    OP_ENV,           // pops name string; pushes value string (or "")
    OP_INPUT_LINE,    // operand: 0=no prompt; 1=has prompt. Pops [prompt?]; pushes input string (no trailing newline)
    OP_PROC_RUN,      // pops command string; pushes map {"out": string, "code": int}
    OP_PROC_SYSTEM,   // pops command string; pushes exit code number
    OP_TIME_NOW_MS,   // pushes current wall-clock time in milliseconds since Unix epoch
    OP_CLOCK_MONO_MS, // pushes monotonic clock in milliseconds (not wall time)
    OP_DATE_FORMAT,   // pops fmt string, ms epoch (int); pushes formatted date string using strftime

    // Threads
    OP_THREAD_SPAWN,  // operand: 0=no args, 1=has args; pops [args?], fn; pushes thread id (int>0)
    OP_THREAD_JOIN,   // pops thread id; waits; pushes result value (or Nil)
    OP_SLEEP_MS,      // pops milliseconds; sleeps; pushes Nil (for statement POP safety)

    // Bitwise (32-bit) and shifts/rotates
    OP_BAND,          // pops b, a; pushes (uint32_t)(a & b)
    OP_BOR,           // pops b, a; pushes (uint32_t)(a | b)
    OP_BXOR,          // pops b, a; pushes (uint32_t)(a ^ b)
    OP_BNOT,          // pops a;   pushes (uint32_t)(~a)
    OP_SHL,           // pops s, a; pushes (uint32_t)(a << (s&31))
    OP_SHR,           // pops s, a; pushes (uint32_t)(a >> (s&31)) logical
    OP_ROTL,          // pops s, a; pushes rotl32(a, s)
    OP_ROTR,          // pops s, a; pushes rotr32(a, s)

    // JSON (json-c)
    OP_JSON_PARSE,       // pops text string; pushes value (or Nil on error)
    OP_JSON_STRINGIFY,   // pops pretty(bool), value; pushes string
    OP_JSON_FROM_FILE,   // pops path string; pushes value (or Nil)
    OP_JSON_TO_FILE,     // pops pretty(bool), value, path; pushes 1/0

    // CURL (libcurl)
    OP_CURL_GET,         // pops [headers map?], url; pushes response string (or "")
    OP_CURL_POST,        // pops [headers map?], body string, url; pushes response string (or "")
    OP_CURL_DOWNLOAD,    // pops [headers map?], path, url; pushes 1/0

    // SQLite (optional)
    OP_SQLITE_OPEN,      // pops path; pushes handle (>0) or 0
    OP_SQLITE_CLOSE,     // pops handle; pushes Nil
    OP_SQLITE_EXEC,      // pops sql, handle; pushes sqlite rc (0=OK)
    OP_SQLITE_QUERY,     // pops sql, handle; pushes array<map>

    // PCSC (smart card) opcodes
    OP_PCSC_ESTABLISH,    // returns context id (>0) or 0
    OP_PCSC_RELEASE,      // pops ctx id; returns 1/0
    OP_PCSC_LIST_READERS, // pops ctx id; returns array of reader names (possibly empty)
    OP_PCSC_CONNECT,      // pops reader, ctx id; returns handle id (>0) or 0
    OP_PCSC_DISCONNECT,   // pops handle id; returns 1/0
    OP_PCSC_TRANSMIT,     // pops apdu array, handle id; returns map {"data":[],"sw1":n,"sw2":n,"code":n}

    // PCRE2 regex ops (optional)
    OP_PCRE2_TEST,        // pops flags, text, pattern; pushes 1/0
    OP_PCRE2_MATCH,       // pops flags, text, pattern; pushes match map or Nil
    OP_PCRE2_FINDALL,     // pops flags, text, pattern; pushes array of match maps

    // Sockets (UNIX platforms)
    OP_SOCK_TCP_LISTEN,    // pops backlog, port; returns listen fd (>0) or 0
    OP_SOCK_TCP_ACCEPT,    // pops listen fd; returns client fd (>0) or 0
    OP_SOCK_TCP_CONNECT,   // pops port, host; returns fd (>0) or 0
    OP_SOCK_SEND,          // pops data string, fd; returns bytes sent (>=0) or -1
    OP_SOCK_RECV,          // pops maxlen, fd; returns data string ("" on EOF/error)
    OP_SOCK_CLOSE,         // pops fd; returns 1/0
    OP_SOCK_UNIX_LISTEN,   // pops backlog, path; returns listen fd (>0) or 0
    OP_SOCK_UNIX_CONNECT,  // pops path; returns fd (>0) or 0

    // process control
    OP_EXIT                // pops code (or uses operand) and terminates script with exit code
} OpCode;

typedef struct {
    OpCode op;
    int32_t operand;
} Instruction;

typedef struct Bytecode {
    Instruction *instructions;
    int instr_count;

    Value *constants;
    int const_count;

    /* debug metadata */
    const char *name;        /* function or module name (optional) */
    const char *source_file; /* originating source filename (optional) */
} Bytecode;

// constructors / manipulation
Bytecode *bytecode_new(void);
int bytecode_add_constant(Bytecode *bc, Value v); /* stores copy */
int bytecode_add_instruction(Bytecode *bc, OpCode op, int32_t operand);
void bytecode_set_operand(Bytecode *bc, int idx, int32_t operand); /* patching */
void bytecode_free(Bytecode *bc);

// utilities
void bytecode_dump(const Bytecode *bc);

#endif
