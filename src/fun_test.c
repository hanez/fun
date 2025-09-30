/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

#include "bytecode.h"
#include "value.h"
#include "vm.h"
#include <stdio.h>

#define ASSERT_EQ(val, expected) \
    if ((val).type != VAL_INT || (val).i != (expected)) { \
        fprintf(stderr, "Assertion failed: expected %lld, got ", (long long)(expected)); \
        print_value(&(val)); \
        printf("\n"); \
        return 1; \
    }

int main(void) {
    VM vm;
    vm_init(&vm);

    Bytecode *bc = bytecode_new();

    // constants
    int c0 = bytecode_add_constant(bc, make_int(0));
    int c1 = bytecode_add_constant(bc, make_int(1));
    int c2 = bytecode_add_constant(bc, make_int(2));
    int c3 = bytecode_add_constant(bc, make_int(3));
    int c10 = bytecode_add_constant(bc, make_int(10));
    int c42 = bytecode_add_constant(bc, make_int(42));

    // ---------- Arithmetic ----------
    bytecode_add_instruction(bc, OP_LOAD_CONST, c42);
    bytecode_add_instruction(bc, OP_LOAD_CONST, c1);
    bytecode_add_instruction(bc, OP_ADD, 0);        // 42+1=43
    bytecode_add_instruction(bc, OP_PRINT, 0);

    bytecode_add_instruction(bc, OP_LOAD_CONST, c10);
    bytecode_add_instruction(bc, OP_LOAD_CONST, c3);
    bytecode_add_instruction(bc, OP_SUB, 0);        // 10-3=7
    bytecode_add_instruction(bc, OP_PRINT, 0);

    bytecode_add_instruction(bc, OP_LOAD_CONST, c2);
    bytecode_add_instruction(bc, OP_LOAD_CONST, c3);
    bytecode_add_instruction(bc, OP_MUL, 0);        // 2*3=6
    bytecode_add_instruction(bc, OP_PRINT, 0);

    bytecode_add_instruction(bc, OP_LOAD_CONST, c10);
    bytecode_add_instruction(bc, OP_LOAD_CONST, c2);
    bytecode_add_instruction(bc, OP_DIV, 0);        // 10/2=5
    bytecode_add_instruction(bc, OP_PRINT, 0);

    bytecode_add_instruction(bc, OP_LOAD_CONST, c10);
    bytecode_add_instruction(bc, OP_LOAD_CONST, c3);
    bytecode_add_instruction(bc, OP_MOD, 0);        // 10%3=1
    bytecode_add_instruction(bc, OP_PRINT, 0);

    // ---------- Comparisons ----------
    bytecode_add_instruction(bc, OP_LOAD_CONST, c1);
    bytecode_add_instruction(bc, OP_LOAD_CONST, c2);
    bytecode_add_instruction(bc, OP_LT, 0);         // 1<2=1
    bytecode_add_instruction(bc, OP_PRINT, 0);

    bytecode_add_instruction(bc, OP_LOAD_CONST, c2);
    bytecode_add_instruction(bc, OP_LOAD_CONST, c2);
    bytecode_add_instruction(bc, OP_LTE, 0);        // 2<=2=1
    bytecode_add_instruction(bc, OP_PRINT, 0);

    bytecode_add_instruction(bc, OP_LOAD_CONST, c3);
    bytecode_add_instruction(bc, OP_LOAD_CONST, c2);
    bytecode_add_instruction(bc, OP_GT, 0);         // 3>2=1
    bytecode_add_instruction(bc, OP_PRINT, 0);

    bytecode_add_instruction(bc, OP_LOAD_CONST, c2);
    bytecode_add_instruction(bc, OP_LOAD_CONST, c2);
    bytecode_add_instruction(bc, OP_GTE, 0);        // 2>=2=1
    bytecode_add_instruction(bc, OP_PRINT, 0);

    bytecode_add_instruction(bc, OP_LOAD_CONST, c2);
    bytecode_add_instruction(bc, OP_LOAD_CONST, c2);
    bytecode_add_instruction(bc, OP_EQ, 0);         // 2==2=1
    bytecode_add_instruction(bc, OP_PRINT, 0);

    bytecode_add_instruction(bc, OP_LOAD_CONST, c2);
    bytecode_add_instruction(bc, OP_LOAD_CONST, c3);
    bytecode_add_instruction(bc, OP_NEQ, 0);        // 2!=3=1
    bytecode_add_instruction(bc, OP_PRINT, 0);

    // ---------- Logical ----------
    bytecode_add_instruction(bc, OP_LOAD_CONST, c1);
    bytecode_add_instruction(bc, OP_LOAD_CONST, c0);
    bytecode_add_instruction(bc, OP_AND, 0);        // 1&&0=0
    bytecode_add_instruction(bc, OP_PRINT, 0);

    bytecode_add_instruction(bc, OP_LOAD_CONST, c1);
    bytecode_add_instruction(bc, OP_LOAD_CONST, c0);
    bytecode_add_instruction(bc, OP_OR, 0);         // 1||0=1
    bytecode_add_instruction(bc, OP_PRINT, 0);

    bytecode_add_instruction(bc, OP_LOAD_CONST, c0);
    bytecode_add_instruction(bc, OP_NOT, 0);        // !0=1
    bytecode_add_instruction(bc, OP_PRINT, 0);

    // ---------- Stack ----------
    bytecode_add_instruction(bc, OP_LOAD_CONST, c1);
    bytecode_add_instruction(bc, OP_DUP, 0);        // duplicate 1
    bytecode_add_instruction(bc, OP_ADD, 0);        // 1+1=2
    bytecode_add_instruction(bc, OP_PRINT, 0);

    bytecode_add_instruction(bc, OP_LOAD_CONST, c1);
    bytecode_add_instruction(bc, OP_LOAD_CONST, c2);
    bytecode_add_instruction(bc, OP_SWAP, 0);       // swap top two
    bytecode_add_instruction(bc, OP_PRINT, 0);      // top=1

    bytecode_add_instruction(bc, OP_LOAD_CONST, c1);
    bytecode_add_instruction(bc, OP_POP, 0);        // dApache-2.0ard 1 (stack now empty)

    // ---------- HALT ----------
    bytecode_add_instruction(bc, OP_HALT, 0);

    printf("=== Bytecode dump ===\n");
    for (int i = 0; i < bc->instr_count; ++i) {
        Instruction instr = bc->instructions[i];
        printf("instr %3d: opcode=%2d operand=%d\n", i, instr.op, instr.operand);
    }
    printf("=====================\n");

    // run VM
    vm_run(&vm, bc);

    printf("All tests executed. Output count: %d\n", vm.output_count);

    vm_clear_output(&vm);
    bytecode_free(bc);
    return 0;
}
