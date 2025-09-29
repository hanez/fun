/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the ISC license.
 * https://opensource.org/license/isc-license-txt
 */

#include "bytecode.h"
#include <stdlib.h>
#include <stdio.h>

Bytecode *bytecode_new(void) {
    Bytecode *bc = (Bytecode*)malloc(sizeof(Bytecode));
    bc->instructions = NULL;
    bc->instr_count = 0;
    bc->constants = NULL;
    bc->const_count = 0;
    return bc;
}

int bytecode_add_constant(Bytecode *bc, Value v) {
    bc->constants = (Value*)realloc(bc->constants, sizeof(Value) * (bc->const_count + 1));
    bc->constants[bc->const_count] = copy_value(&v);
    return bc->const_count++;
}

int bytecode_add_instruction(Bytecode *bc, OpCode op, int32_t operand) {
    bc->instructions = (Instruction*)realloc(bc->instructions, sizeof(Instruction) * (bc->instr_count + 1));
    bc->instructions[bc->instr_count].op = op;
    bc->instructions[bc->instr_count].operand = operand;
    return bc->instr_count++;
}

void bytecode_set_operand(Bytecode *bc, int idx, int32_t operand) {
    if (idx >= 0 && idx < bc->instr_count) {
        bc->instructions[idx].operand = operand;
    }
}

void bytecode_free(Bytecode *bc) {
    if (!bc) return;
    for (int i = 0; i < bc->const_count; ++i) {
        free_value(bc->constants[i]);
    }
    free(bc->constants);
    free(bc->instructions);
    free(bc);
}

static const char *opcode_name(OpCode op) {
    switch (op) {
        case OP_NOP: return "NOP";
        case OP_LOAD_CONST: return "LOAD_CONST";
        case OP_LOAD_LOCAL: return "LOAD_LOCAL";
        case OP_STORE_LOCAL: return "STORE_LOCAL";
        case OP_LOAD_GLOBAL: return "LOAD_GLOBAL";
        case OP_STORE_GLOBAL: return "STORE_GLOBAL";
        case OP_ADD: return "ADD";
        case OP_SUB: return "SUB";
        case OP_MUL: return "MUL";
        case OP_DIV: return "DIV";
        case OP_LT: return "LT";
        case OP_LTE: return "LTE";
        case OP_GT: return "GT";
        case OP_GTE: return "GTE";
        case OP_EQ: return "EQ";
        case OP_NEQ: return "NEQ";
        case OP_POP: return "POP";
        case OP_JUMP: return "JUMP";
        case OP_JUMP_IF_FALSE: return "JUMP_IF_FALSE";
        case OP_CALL: return "CALL";
        case OP_RETURN: return "RETURN";
        case OP_PRINT: return "PRINT";
        case OP_HALT: return "HALT";
        case OP_MOD: return "MOD";
        case OP_AND: return "AND";
        case OP_OR: return "OR";
        case OP_NOT: return "NOT";
        case OP_DUP: return "DUP";
        case OP_SWAP: return "SWAP";
        case OP_MAKE_ARRAY: return "MAKE_ARRAY";
        case OP_INDEX_GET: return "INDEX_GET";
        case OP_INDEX_SET: return "INDEX_SET";
        case OP_LEN: return "LEN";
        case OP_PUSH: return "ARR_PUSH";
        case OP_APOP: return "ARR_POP";
        case OP_SET: return "ARR_SET";
        case OP_INSERT: return "ARR_INSERT";
        case OP_REMOVE: return "ARR_REMOVE";
        case OP_SLICE: return "SLICE";
        case OP_TO_NUMBER: return "TO_NUMBER";
        case OP_TO_STRING: return "TO_STRING";
        case OP_TYPEOF: return "TYPEOF";
        case OP_CAST: return "CAST";
        case OP_SPLIT: return "SPLIT";
        case OP_JOIN: return "JOIN";
        case OP_SUBSTR: return "SUBSTR";
        case OP_FIND: return "FIND";
        case OP_CONTAINS: return "CONTAINS";
        case OP_INDEX_OF: return "INDEX_OF";
        case OP_CLEAR: return "CLEAR";
        case OP_ENUMERATE: return "ENUMERATE";
        case OP_ZIP: return "ZIP";
        case OP_MIN: return "MIN";
        case OP_MAX: return "MAX";
        case OP_CLAMP: return "CLAMP";
        case OP_ABS: return "ABS";
        case OP_POW: return "POW";
        case OP_RANDOM_SEED: return "RANDOM_SEED";
        case OP_RANDOM_INT: return "RANDOM_INT";
        case OP_MAKE_MAP: return "MAKE_MAP";
        case OP_KEYS: return "KEYS";
        case OP_VALUES: return "VALUES";
        case OP_HAS_KEY: return "HAS_KEY";
        case OP_READ_FILE: return "READ_FILE";
        case OP_WRITE_FILE: return "WRITE_FILE";
        default: return "???";
    }
}

void bytecode_dump(const Bytecode *bc) {
    if (!bc) {
        printf("<null bytecode>\n");
        return;
    }
    printf("Constants (%d):\n", bc->const_count);
    for (int i = 0; i < bc->const_count; ++i) {
        printf("  [%d] ", i);
        print_value(&bc->constants[i]);
        printf("\n");
    }
    printf("Instructions (%d):\n", bc->instr_count);
    for (int i = 0; i < bc->instr_count; ++i) {
        const Instruction *ins = &bc->instructions[i];
        printf("  %3d: %-15s %d\n", i, opcode_name(ins->op), ins->operand);
    }
}
