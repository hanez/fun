/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the ISC license.
 * https://opensource.org/license/isc-license-txt
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

    OP_POP,            // discard top of stack
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
    OP_TYPEOF,        // pops any; pushes string name of type
    OP_UCLAMP,        // pops number; pushes number masked to N bits (operand = bits)
    OP_SCLAMP,        // pops number; pushes number clamped to signed N-bit range (operand = bits)

    // string ops
    OP_SPLIT,         // pops sep, string; pushes array of strings
    OP_JOIN,          // pops sep, array; pushes string
    OP_SUBSTR,        // pops len, start, string; pushes string
    OP_FIND,          // pops needle, haystack; pushes int index or -1

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
    OP_ENV            // pops name string; pushes value string (or "")
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
