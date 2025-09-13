#include "vm.h"
#include "bytecode.h"
#include "value.h"
#include <stdio.h>

int main() {
    Bytecode *bc = bytecode_new();

    // Example: test OP_ADD
    int c1 = bytecode_add_constant(bc, make_int(5));
    int c2 = bytecode_add_constant(bc, make_int(3));
    bytecode_add_instruction(bc, OP_CONSTANT, c1);
    bytecode_add_instruction(bc, OP_CONSTANT, c2);
    bytecode_add_instruction(bc, OP_ADD, 0);
    bytecode_add_instruction(bc, OP_PRINT, 0);

    VM vm;
    vm_init(&vm);

    printf("=== Bytecode dump ===\n");
    bytecode_dump(bc);

    if (vm_run(&vm, bc) != 0) {
        printf("Runtime error!\n");
    }

    printf("Output count: %d\n", vm.output_count);
    for (int i = 0; i < vm.output_count; i++) {
        printf("Output[%d] = %ld\n", i, vm.output[i].i);
    }

    vm_free(&vm);
    bytecode_free(bc);
    return 0;
}

