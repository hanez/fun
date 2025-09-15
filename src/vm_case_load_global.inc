case OP_LOAD_GLOBAL: {
    int idx = inst.operand;
    if (idx < 0 || idx >= VM_MAX_GLOBALS) {
        fprintf(stderr, "Runtime error: global index out of range\n");
        exit(1);
    }
    push_value(vm, copy_value(&vm->globals[idx]));
    break;
}
