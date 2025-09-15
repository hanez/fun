case OP_RETURN: {
    Value retv;
    if (vm->sp >= 0) retv = pop_value(vm);
    else retv = make_nil();
    vm_pop_frame(vm);
    push_value(vm, retv);
    break;
}
