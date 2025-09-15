case OP_JUMP_IF_FALSE: {
    Value cond = pop_value(vm);
    int truthy = value_is_truthy(&cond);
    free_value(cond);
    if (!truthy) {
        f->ip = inst.operand;
    }
    break;
}
