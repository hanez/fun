case OP_FIND: {
    Value needle = pop_value(vm);
    Value hay = pop_value(vm);
    if (hay.type != VAL_STRING || needle.type != VAL_STRING) {
        fprintf(stderr, "Runtime type error: FIND expects (string, string)\n");
        exit(1);
    }
    int idx = bi_find(&hay, &needle);
    free_value(hay);
    free_value(needle);
    push_value(vm, make_int(idx));
    break;
}
