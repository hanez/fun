case OP_READ_FILE: {
    Value path = pop_value(vm);
    if (path.type != VAL_STRING) { fprintf(stderr, "READ_FILE expects string\n"); exit(1); }
    const char *p = path.s ? path.s : "";
    FILE *f = fopen(p, "rb");
    if (!f) { free_value(path); push_value(vm, make_string("")); break; }
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); free_value(path); push_value(vm, make_string("")); break; }
    long sz = ftell(f);
    if (sz < 0) { fclose(f); free_value(path); push_value(vm, make_string("")); break; }
    rewind(f);
    char *buf = (char*)malloc((size_t)sz + 1);
    size_t n = buf ? fread(buf, 1, (size_t)sz, f) : 0;
    fclose(f);
    if (!buf) { free_value(path); push_value(vm, make_string("")); break; }
    buf[n] = '\0';
    Value out = make_string(buf);
    free(buf);
    free_value(path);
    push_value(vm, out);
    break;
}
