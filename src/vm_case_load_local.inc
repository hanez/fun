case OP_LOAD_LOCAL: {
    int slot = inst.operand;
    if (slot < 0 || slot >= FRAME_MAX_LOCALS) {
        fprintf(stderr, "Runtime error: local slot out of range\n");
        exit(1);
    }
    Value val = copy_value(&f->locals[slot]);
    push_value(vm, val);
    break;
}
