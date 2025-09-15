case OP_GT: {
    Value b = pop_value(vm);
    Value a = pop_value(vm);
    if (a.type != VAL_INT || b.type != VAL_INT) {
        fprintf(stderr, "Runtime type error: GT expects ints\n");
        exit(1);
    }
    push_value(vm, make_int(a.i > b.i ? 1 : 0));
    free_value(a); free_value(b);
    break;
}
