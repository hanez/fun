case OP_LOAD_CONST: {
    int idx = inst.operand;
    if (idx < 0 || idx >= f->fn->const_count) {
        fprintf(stderr, "Runtime error: constant index out of range\n");
        exit(1);
    }
    Value c = copy_value(&f->fn->constants[idx]);
    push_value(vm, c);
    break;
}
