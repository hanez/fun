case OP_LOAD_GLOBAL: {
    int idx = inst.operand;
    if (idx < 0 || idx >= VM_MAX_GLOBALS) {
        fprintf(stderr, "Runtime error: global index out of range\n");
        exit(1);
    }
#ifdef FUN_DEBUG
    fprintf(stderr, "DEBUG LOAD_GLOBAL[%d]: type=%d\n", idx, vm->globals[idx].type);
#endif
    push_value(vm, copy_value(&vm->globals[idx]));
    break;
}
