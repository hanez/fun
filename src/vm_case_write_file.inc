case OP_WRITE_FILE: {
    Value data = pop_value(vm);
    Value path = pop_value(vm);
    if (path.type != VAL_STRING || data.type != VAL_STRING) { fprintf(stderr, "WRITE_FILE expects (string, string)\n"); exit(1); }
    const char *p = path.s ? path.s : "";
    FILE *f = fopen(p, "wb");
    int ok = 0;
    if (f) {
        size_t len = data.s ? strlen(data.s) : 0;
        ok = (fwrite(data.s ? data.s : "", 1, len, f) == len);
        fclose(f);
    }
    free_value(path);
    free_value(data);
    push_value(vm, make_int(ok ? 1 : 0));
    break;
}
