case OP_SPLIT: {
    Value sep = pop_value(vm);
    Value str = pop_value(vm);
    if (str.type != VAL_STRING || sep.type != VAL_STRING) {
        fprintf(stderr, "Runtime type error: SPLIT expects (string, string)\n");
        exit(1);
    }
    Value out = bi_split(&str, &sep);
    free_value(str);
    free_value(sep);
    push_value(vm, out);
    break;
}
