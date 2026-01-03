/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

#include "bytecode.h"
#include "value.h"
#include "vm.h"
#include <stdio.h>
#include <math.h>

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
    int cf3_2 = bytecode_add_constant(bc, make_float(3.2));
    int cf3_5 = bytecode_add_constant(bc, make_float(3.5));
    int cf3_8 = bytecode_add_constant(bc, make_float(3.8));
    int cfn3_2 = bytecode_add_constant(bc, make_float(-3.2));
    int cfn3_5 = bytecode_add_constant(bc, make_float(-3.5));
    int cfn3_8 = bytecode_add_constant(bc, make_float(-3.8));
    int cf0 = bytecode_add_constant(bc, make_float(0.0));
    int cf1 = bytecode_add_constant(bc, make_float(1.0));
    int cf5 = bytecode_add_constant(bc, make_float(5.0));
    int c4 = bytecode_add_constant(bc, make_int(4));
    int c9 = bytecode_add_constant(bc, make_int(9));
    int c48 = bytecode_add_constant(bc, make_int(48));
    int c18 = bytecode_add_constant(bc, make_int(18));
    int c21 = bytecode_add_constant(bc, make_int(21));
    int c6 = bytecode_add_constant(bc, make_int(6));
    int c15 = bytecode_add_constant(bc, make_int(15));
    int c16 = bytecode_add_constant(bc, make_int(16));
    int cneg5 = bytecode_add_constant(bc, make_int(-5));
    int c7 = bytecode_add_constant(bc, make_int(7));

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

    // ---------- Rounding (math.h) demo ----------
    // floor/ceil/trunc/round on representative values
    int start_round_demo = bc->instr_count;
    (void)start_round_demo;

    // +3.2
    bytecode_add_instruction(bc, OP_LOAD_CONST, cf3_2);
    bytecode_add_instruction(bc, OP_FLOOR, 0);
    bytecode_add_instruction(bc, OP_PRINT, 0);
    bytecode_add_instruction(bc, OP_LOAD_CONST, cf3_2);
    bytecode_add_instruction(bc, OP_CEIL, 0);
    bytecode_add_instruction(bc, OP_PRINT, 0);
    bytecode_add_instruction(bc, OP_LOAD_CONST, cf3_2);
    bytecode_add_instruction(bc, OP_TRUNC, 0);
    bytecode_add_instruction(bc, OP_PRINT, 0);
    bytecode_add_instruction(bc, OP_LOAD_CONST, cf3_2);
    bytecode_add_instruction(bc, OP_ROUND, 0);
    bytecode_add_instruction(bc, OP_PRINT, 0);

    // +3.5
    bytecode_add_instruction(bc, OP_LOAD_CONST, cf3_5);
    bytecode_add_instruction(bc, OP_ROUND, 0);
    bytecode_add_instruction(bc, OP_PRINT, 0);

    // -3.5
    bytecode_add_instruction(bc, OP_LOAD_CONST, cfn3_5);
    bytecode_add_instruction(bc, OP_ROUND, 0);
    bytecode_add_instruction(bc, OP_PRINT, 0);

    // -3.2
    bytecode_add_instruction(bc, OP_LOAD_CONST, cfn3_2);
    bytecode_add_instruction(bc, OP_FLOOR, 0);
    bytecode_add_instruction(bc, OP_PRINT, 0);
    bytecode_add_instruction(bc, OP_LOAD_CONST, cfn3_2);
    bytecode_add_instruction(bc, OP_CEIL, 0);
    bytecode_add_instruction(bc, OP_PRINT, 0);

    // integers should be unchanged
    bytecode_add_instruction(bc, OP_LOAD_CONST, c10);
    bytecode_add_instruction(bc, OP_FLOOR, 0);
    bytecode_add_instruction(bc, OP_PRINT, 0);

    // ---------- Transcendentals demo ----------
    // sin(0)=0
    bytecode_add_instruction(bc, OP_LOAD_CONST, cf0);
    bytecode_add_instruction(bc, OP_SIN, 0);
    bytecode_add_instruction(bc, OP_PRINT, 0);
    // cos(0)=1
    bytecode_add_instruction(bc, OP_LOAD_CONST, cf0);
    bytecode_add_instruction(bc, OP_COS, 0);
    bytecode_add_instruction(bc, OP_PRINT, 0);
    // tan(0)=0
    bytecode_add_instruction(bc, OP_LOAD_CONST, cf0);
    bytecode_add_instruction(bc, OP_TAN, 0);
    bytecode_add_instruction(bc, OP_PRINT, 0);
    // exp(0)=1
    bytecode_add_instruction(bc, OP_LOAD_CONST, cf0);
    bytecode_add_instruction(bc, OP_EXP, 0);
    bytecode_add_instruction(bc, OP_PRINT, 0);
    // log(1)=0
    bytecode_add_instruction(bc, OP_LOAD_CONST, cf1);
    bytecode_add_instruction(bc, OP_LOG, 0);
    bytecode_add_instruction(bc, OP_PRINT, 0);
    // log10(1)=0
    bytecode_add_instruction(bc, OP_LOAD_CONST, cf1);
    bytecode_add_instruction(bc, OP_LOG10, 0);
    bytecode_add_instruction(bc, OP_PRINT, 0);
    // sqrt(9)=3
    bytecode_add_instruction(bc, OP_LOAD_CONST, c9);
    bytecode_add_instruction(bc, OP_SQRT, 0);
    bytecode_add_instruction(bc, OP_PRINT, 0);

    // ---------- Integer math (gcd/lcm/isqrt/sign) demo ----------
    // gcd(48,18)=6
    bytecode_add_instruction(bc, OP_LOAD_CONST, c48);
    bytecode_add_instruction(bc, OP_LOAD_CONST, c18);
    bytecode_add_instruction(bc, OP_GCD, 0);
    bytecode_add_instruction(bc, OP_PRINT, 0);

    // lcm(21,6)=42
    bytecode_add_instruction(bc, OP_LOAD_CONST, c21);
    bytecode_add_instruction(bc, OP_LOAD_CONST, c6);
    bytecode_add_instruction(bc, OP_LCM, 0);
    bytecode_add_instruction(bc, OP_PRINT, 0);

    // isqrt cases: 0, 1, 15->3, 16->4
    bytecode_add_instruction(bc, OP_LOAD_CONST, c0);
    bytecode_add_instruction(bc, OP_ISQRT, 0);
    bytecode_add_instruction(bc, OP_PRINT, 0);
    bytecode_add_instruction(bc, OP_LOAD_CONST, c1);
    bytecode_add_instruction(bc, OP_ISQRT, 0);
    bytecode_add_instruction(bc, OP_PRINT, 0);
    bytecode_add_instruction(bc, OP_LOAD_CONST, c15);
    bytecode_add_instruction(bc, OP_ISQRT, 0);
    bytecode_add_instruction(bc, OP_PRINT, 0);
    bytecode_add_instruction(bc, OP_LOAD_CONST, c16);
    bytecode_add_instruction(bc, OP_ISQRT, 0);
    bytecode_add_instruction(bc, OP_PRINT, 0);

    // sign(-5)=-1, sign(0)=0, sign(7)=1
    bytecode_add_instruction(bc, OP_LOAD_CONST, cneg5);
    bytecode_add_instruction(bc, OP_SIGN, 0);
    bytecode_add_instruction(bc, OP_PRINT, 0);
    bytecode_add_instruction(bc, OP_LOAD_CONST, c0);
    bytecode_add_instruction(bc, OP_SIGN, 0);
    bytecode_add_instruction(bc, OP_PRINT, 0);
    bytecode_add_instruction(bc, OP_LOAD_CONST, c7);
    bytecode_add_instruction(bc, OP_SIGN, 0);
    bytecode_add_instruction(bc, OP_PRINT, 0);

    // ---------- fmin/fmax demo ----------
    // fmin(3.2, 4) -> 3.2
    bytecode_add_instruction(bc, OP_LOAD_CONST, cf3_2);
    bytecode_add_instruction(bc, OP_LOAD_CONST, c4);
    bytecode_add_instruction(bc, OP_FMIN, 0);
    bytecode_add_instruction(bc, OP_PRINT, 0);
    // fmax(3.2, 4) -> 4
    bytecode_add_instruction(bc, OP_LOAD_CONST, cf3_2);
    bytecode_add_instruction(bc, OP_LOAD_CONST, c4);
    bytecode_add_instruction(bc, OP_FMAX, 0);
    bytecode_add_instruction(bc, OP_PRINT, 0);
    // NaN cases
    double nanv = NAN;
    int cNaN = bytecode_add_constant(bc, make_float(nanv));
    // fmin(NaN, 5.0) -> 5.0
    bytecode_add_instruction(bc, OP_LOAD_CONST, cNaN);
    bytecode_add_instruction(bc, OP_LOAD_CONST, cf5);
    bytecode_add_instruction(bc, OP_FMIN, 0);
    bytecode_add_instruction(bc, OP_PRINT, 0);
    // fmax(NaN, 5.0) -> 5.0
    bytecode_add_instruction(bc, OP_LOAD_CONST, cNaN);
    bytecode_add_instruction(bc, OP_LOAD_CONST, cf5);
    bytecode_add_instruction(bc, OP_FMAX, 0);
    bytecode_add_instruction(bc, OP_PRINT, 0);
    // fmin(5.0, NaN) -> 5.0
    bytecode_add_instruction(bc, OP_LOAD_CONST, cf5);
    bytecode_add_instruction(bc, OP_LOAD_CONST, cNaN);
    bytecode_add_instruction(bc, OP_FMIN, 0);
    bytecode_add_instruction(bc, OP_PRINT, 0);
    // fmax(5.0, NaN) -> 5.0
    bytecode_add_instruction(bc, OP_LOAD_CONST, cf5);
    bytecode_add_instruction(bc, OP_LOAD_CONST, cNaN);
    bytecode_add_instruction(bc, OP_FMAX, 0);
    bytecode_add_instruction(bc, OP_PRINT, 0);
    // fmin(NaN, NaN) -> NaN (prints as nan)
    bytecode_add_instruction(bc, OP_LOAD_CONST, cNaN);
    bytecode_add_instruction(bc, OP_LOAD_CONST, cNaN);
    bytecode_add_instruction(bc, OP_FMIN, 0);
    bytecode_add_instruction(bc, OP_PRINT, 0);

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
