case OP_LEN: {
    Value a = pop_value(vm);
    int len = 0;
    if (a.type == VAL_STRING) {
        len = (int)(a.s ? (int)strlen(a.s) : 0);
    } else if (a.type == VAL_ARRAY) {
        len = array_length(&a);
        if (len < 0) len = 0;
    } else {
        fprintf(stderr, "Runtime type error: LEN expects array or string\n");
        exit(1);
    }
    free_value(a);
    push_value(vm, make_int(len));
    break;
}
