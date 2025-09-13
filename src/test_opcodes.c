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
    vm_free(&vm);
    bytecode_free(bc);
    return 0;
}
