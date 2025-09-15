case OP_ARR_INSERT: {
    Value v = pop_value(vm);
    Value idx = pop_value(vm);
    Value arr = pop_value(vm);
    if (arr.type != VAL_ARRAY || idx.type != VAL_INT) {
        fprintf(stderr, "Runtime type error: ARR_INSERT expects (array, int, value)\n");
        exit(1);
    }
    int n = array_insert(&arr, (int)idx.i, v);
    if (n < 0) {
        fprintf(stderr, "Runtime error: insert failed (OOM?)\n");
        exit(1);
    }
    free_value(arr);
    free_value(idx);
    push_value(vm, make_int(n));
    break;
}
