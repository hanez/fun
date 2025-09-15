case OP_DUP: {
    if (vm->sp < 0) {
        fprintf(stderr, "Runtime error: stack underflow for DUP\n");
        exit(1);
    }
    Value top = vm->stack[vm->sp];
    push_value(vm, copy_value(&top));
    break;
}
