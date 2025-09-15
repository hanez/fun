case OP_SUBSTR: {
    Value lenv = pop_value(vm);
    Value startv = pop_value(vm);
    Value str = pop_value(vm);
    if (str.type != VAL_STRING || startv.type != VAL_INT || lenv.type != VAL_INT) {
        fprintf(stderr, "Runtime type error: SUBSTR expects (string, int, int)\n");
        exit(1);
    }
    Value out = bi_substr(&str, (int)startv.i, (int)lenv.i);
    free_value(str);
    free_value(startv);
    free_value(lenv);
    push_value(vm, out);
    break;
}
