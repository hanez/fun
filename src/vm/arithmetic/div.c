case OP_DIV: {
    Value b = pop_value(vm);
    Value a = pop_value(vm);
    if (a.type != VAL_INT || b.type != VAL_INT) {
        fprintf(stderr, "Runtime type error: DIV expects ints, got %s and %s\n",
                value_type_name(a.type), value_type_name(b.type));
        exit(1);
    }
    if (b.i == 0) {
        fprintf(stderr, "Runtime error: division by zero\n");
        exit(1);
    }
    Value res = make_int(a.i / b.i);
    free_value(a);
    free_value(b);
    push_value(vm, res);
    break;
}
