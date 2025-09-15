case OP_ZIP: {
    Value b = pop_value(vm);
    Value a = pop_value(vm);
    if (a.type != VAL_ARRAY || b.type != VAL_ARRAY) {
        fprintf(stderr, "Runtime type error: ZIP expects (array, array)\n");
        exit(1);
    }
    Value out = bi_zip(&a, &b);
    free_value(a);
    free_value(b);
    push_value(vm, out);
    break;
}
