case OP_ENUMERATE: {
    Value arr = pop_value(vm);
    if (arr.type != VAL_ARRAY) {
        fprintf(stderr, "Runtime type error: ENUMERATE expects array\n");
        exit(1);
    }
    Value out = bi_enumerate(&arr);
    free_value(arr);
    push_value(vm, out);
    break;
}
