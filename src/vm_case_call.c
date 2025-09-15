case OP_CALL: {
    int argc = inst.operand;
    if (argc < 0) argc = 0;
    /* collect args in reverse (preserve order) */
    Value *args = NULL;
    if (argc > 0) {
        args = (Value*)malloc(sizeof(Value) * argc);
        /* pop args into array in reverse */
        for (int i = argc - 1; i >= 0; --i) {
            args[i] = pop_value(vm);
        }
    }
    /* pop function value */
    Value fnv = pop_value(vm);
    if (fnv.type != VAL_FUNCTION) {
        fprintf(stderr, "Runtime type error: CALL expects function\n");
        exit(1);
    }
    /* push new frame and transfer args */
    vm_push_frame(vm, fnv.fn, argc, args);
    /* free args array (locals moved), free fnv (no-op for function) */
    free(args);
    /* note: fnv contains a pointer to the Bytecode, don't free here */
    break;
}
