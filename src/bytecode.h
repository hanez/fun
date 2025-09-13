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

    // add after existing opcodes
    OP_MOD,          // a % b
    OP_AND,      // logical AND
    OP_OR,       // logical OR
    OP_NOT,      // logical NOT

    OP_DUP,          // duplicate top of stack
    OP_SWAP,         // swap top two stack values
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

#endif

