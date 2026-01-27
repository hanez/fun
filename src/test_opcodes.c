/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

#include "vm.h"
#include "bytecode.h"
#include "value.h"
#include <stdio.h>

int main() {

    VM vm;
    vm_init(&vm);
    
    Bytecode *bc = bytecode_new();

    // Example: test OP_ADD
    int c1 = bytecode_add_constant(bc, make_int(5));
    int c2 = bytecode_add_constant(bc, make_int(3));
    bytecode_add_instruction(bc, OP_LOAD_CONST, c1);
    bytecode_add_instruction(bc, OP_LOAD_CONST, c2);
    bytecode_add_instruction(bc, OP_ADD, 0);
    bytecode_add_instruction(bc, OP_PRINT, 0);

    printf("=== Bytecode dump ===\n");
    for (int i = 0; i < bc->instr_count; ++i) {
        Instruction instr = bc->instructions[i];
        printf("instr %3d: opcode=%2d operand=%d\n", i, instr.op, instr.operand);
    }
    printf("=====================\n");
    bytecode_dump(bc);
    printf("=====================\n");
    
    vm_run(&vm, bc);

    printf("Output count: %d\n", vm.output_count);
    for (int i = 0; i < vm.output_count; i++) {
        printf("Output[%d] = ", i);
        print_value(&vm.output[i]);
        printf("\n");
    }

    vm_clear_output(&vm);

    /* --- Rust FFI demo: call a Rust opcode and string function --- */
#ifdef FUN_WITH_RUST
    extern int fun_op_radd(VM *vm);
    extern const char *fun_rust_get_string(void);

    printf("=== Rust FFI demo ===\n");
    const char *rs = fun_rust_get_string();
    if (rs) {
        printf("Rust says: %s\n", rs);
    }

    /* prepare stack: push 10 and 32, then call Rust add -> expect 42 */
    vm_push_i64(&vm, 10);
    vm_push_i64(&vm, 32);
    int rc = fun_op_radd(&vm);
    printf("fun_op_radd rc=%d\n", rc);
    long long sum = (long long)vm_pop_i64(&vm);
    printf("Rust op result: %lld\n", sum);
#else
    printf("=== Rust FFI demo (disabled; build with -DFUN_WITH_RUST=ON) ===\n");
#endif

    vm_free(&vm);
    bytecode_free(bc);
    return 0;
}
