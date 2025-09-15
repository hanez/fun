case OP_STORE_GLOBAL: {
    int idx = inst.operand;
    if (idx < 0 || idx >= VM_MAX_GLOBALS) {
        fprintf(stderr, "Runtime error: global index out of range\n");
        exit(1);
    }
    Value v = pop_value(vm);
    free_value(vm->globals[idx]);
    vm->globals[idx] = v;
    break;
}
