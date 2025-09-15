case OP_ARR_SET: {
    Value v = pop_value(vm);
    Value idx = pop_value(vm);
    Value arr = pop_value(vm);
    if (arr.type != VAL_ARRAY || idx.type != VAL_INT) {
        fprintf(stderr, "Runtime type error: ARR_SET expects (array, int, value)\n");
        exit(1);
    }
    if (!array_set(&arr, (int)idx.i, v)) {
        fprintf(stderr, "Runtime error: set index out of range\n");
        exit(1);
    }
    free_value(arr);
    free_value(idx);
    /* v already owned by array; push copy for return value */
    push_value(vm, copy_value(&v));
    free_value(v);
    break;
}
