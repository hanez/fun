case OP_STORE_LOCAL: {
    int slot = inst.operand;
    if (slot < 0 || slot >= FRAME_MAX_LOCALS) {
        fprintf(stderr, "Runtime error: local slot out of range\n");
        exit(1);
    }
    Value v = pop_value(vm);
    /* free previous local then move v into it */
    free_value(f->locals[slot]);
    f->locals[slot] = v;
    break;
}
