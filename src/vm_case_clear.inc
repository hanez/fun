case OP_CLEAR: {
    Value arr = pop_value(vm);
    if (arr.type != VAL_ARRAY) {
        fprintf(stderr, "Runtime type error: CLEAR expects array\n");
        exit(1);
    }
    array_clear(&arr);
    free_value(arr);
    push_value(vm, make_int(0));
    break;
}
