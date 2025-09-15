case OP_MAKE_ARRAY: {
    int n = inst.operand;
    if (n < 0 || vm->sp + 1 < n) {
        fprintf(stderr, "Runtime error: invalid element count for MAKE_ARRAY\n");
        exit(1);
    }
    /* pop n values into temp array preserving original order */
    Value *vals = (Value*)malloc(sizeof(Value) * n);
    if (!vals) { fprintf(stderr, "Runtime error: OOM in MAKE_ARRAY\n"); exit(1); }
    for (int i = n - 1; i >= 0; --i) {
        vals[i] = pop_value(vm); /* take ownership */
    }
    /* build array by copying values, then free originals */
    Value arr = make_array_from_values(vals, n);
    for (int i = 0; i < n; ++i) {
        free_value(vals[i]);
    }
    free(vals);
    push_value(vm, arr);
    break;
}
