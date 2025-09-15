case OP_STORE_GLOBAL: {
    int idx = inst.operand;
    if (idx < 0 || idx >= VM_MAX_GLOBALS) {
        fprintf(stderr, "Runtime error: global index out of range\n");
        exit(1);
    }
    Value v = pop_value(vm);
#ifdef FUN_DEBUG
    fprintf(stderr, "DEBUG STORE_GLOBAL[%d]: new.type=%d\n", idx, v.type);
#endif
    free_value(vm->globals[idx]);
    vm->globals[idx] = v;
    break;
}
