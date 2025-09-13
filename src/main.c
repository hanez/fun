#include "bytecode.h"
#include "value.h"
#include "vm.h"
#include <stdio.h>

int main(void) {
    VM vm;
    vm_init(&vm);

    /* Example: 0..4 loop */
    Bytecode *bc = bytecode_new();
    int c0  = bytecode_add_constant(bc, make_int(0));
    int c1  = bytecode_add_constant(bc, make_int(1));
    int c5  = bytecode_add_constant(bc, make_int(5));

    // initialize i = 0
    bytecode_add_instruction(bc, OP_LOAD_CONST, c0);
    bytecode_add_instruction(bc, OP_STORE_LOCAL, 0);

    int lbl_start = bc->instr_count;

    // load i
    bytecode_add_instruction(bc, OP_LOAD_LOCAL, 0);
    bytecode_add_instruction(bc, OP_PRINT, 0);

    // i = i + 1
    bytecode_add_instruction(bc, OP_LOAD_LOCAL, 0);
    bytecode_add_instruction(bc, OP_LOAD_CONST, c1);
    bytecode_add_instruction(bc, OP_ADD, 0);
    bytecode_add_instruction(bc, OP_STORE_LOCAL, 0);

    // check i < 5
    bytecode_add_instruction(bc, OP_LOAD_LOCAL, 0);
    bytecode_add_instruction(bc, OP_LOAD_CONST, c5);
    bytecode_add_instruction(bc, OP_LT, 0);
    int jmp_back = bytecode_add_instruction(bc, OP_JUMP_IF_FALSE, 0);
    bytecode_add_instruction(bc, OP_JUMP, lbl_start);

    int end_lbl = bc->instr_count;
    bytecode_set_operand(bc, jmp_back, end_lbl);

    bytecode_add_instruction(bc, OP_HALT, 0);

    vm_run(&vm, bc);

    // flush captured outputs
    vm_clear_output(&vm);

    bytecode_free(bc);
    return 0;
}

