case OP_TO_STRING: {
    Value v = pop_value(vm);
    char *s = value_to_string_alloc(&v);
    Value out = make_string(s ? s : "");
    if (s) free(s);
    free_value(v);
    push_value(vm, out);
    break;
}
