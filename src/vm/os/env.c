case OP_ENV: {
    Value key = pop_value(vm);
    if (key.type != VAL_STRING) {
        fprintf(stderr, "Runtime type error: ENV expects string name\n");
        free_value(key);
        exit(1);
    }
    const char *name = key.s ? key.s : "";
    const char *val = getenv(name);
    /* Return empty string if not set (consistent with read_file fallback style) */
    push_value(vm, make_string(val ? val : ""));
    free_value(key);
    break;
}
