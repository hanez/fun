case OP_ARR_REMOVE: {
    Value idx = pop_value(vm);
    Value arr = pop_value(vm);
    if (arr.type != VAL_ARRAY || idx.type != VAL_INT) {
        fprintf(stderr, "Runtime type error: ARR_REMOVE expects (array, int)\n");
        exit(1);
    }
    Value out;
    if (!array_remove(&arr, (int)idx.i, &out)) {
        fprintf(stderr, "Runtime error: remove index out of range\n");
        exit(1);
    }
    free_value(arr);
    free_value(idx);
    push_value(vm, out);
    break;
}
