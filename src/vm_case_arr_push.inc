case OP_ARR_PUSH: {
    Value v = pop_value(vm);
    Value arr = pop_value(vm);
    if (arr.type != VAL_ARRAY) {
        fprintf(stderr, "Runtime type error: ARR_PUSH expects array\n");
        exit(1);
    }
    int n = array_push(&arr, v);
    if (n < 0) {
        fprintf(stderr, "Runtime error: push failed (OOM?)\n");
        exit(1);
    }
    free_value(arr);
    push_value(vm, make_int(n));
    break;
}
