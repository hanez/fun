case OP_OR: {
    Value b = pop_value(vm);
    Value a = pop_value(vm);
    int res = value_is_truthy(&a) || value_is_truthy(&b);
    free_value(a);
    free_value(b);
    push_value(vm, make_int(res));
    break;
}
