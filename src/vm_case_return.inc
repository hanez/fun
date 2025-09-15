case OP_RETURN: {
    Value retv;
    /* if there's something on the stack -> return it, else nil */
    if (vm->sp >= 0) retv = pop_value(vm);
    else retv = make_nil();

    /* pop frame (free locals) */
    vm_pop_frame(vm);

    /* push return value into caller frame (or onto stack if no caller) */
    push_value(vm, retv);
    break;
}
