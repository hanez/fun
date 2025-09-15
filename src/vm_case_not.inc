case OP_NOT: {
    Value v = pop_value(vm);
    int res = !value_is_truthy(&v);
    free_value(v);
    push_value(vm, make_int(res));
    break;
}
