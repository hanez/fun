#include "bytecode.h"
#include <stdlib.h>
#include <string.h>
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

