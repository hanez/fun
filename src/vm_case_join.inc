case OP_JOIN: {
    Value sep = pop_value(vm);
    Value arr = pop_value(vm);
    if (arr.type != VAL_ARRAY || sep.type != VAL_STRING) {
        fprintf(stderr, "Runtime type error: JOIN expects (array, string)\n");
        exit(1);
    }
    Value out = bi_join(&arr, &sep);
    free_value(arr);
    free_value(sep);
    push_value(vm, out);
    break;
}
