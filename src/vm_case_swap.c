case OP_SWAP: {
    if (vm->sp < 1) {
        fprintf(stderr, "Runtime error: stack underflow for SWAP\n");
        exit(1);
    }
    Value a = vm->stack[vm->sp];
    Value b = vm->stack[vm->sp - 1];
    vm->stack[vm->sp] = b;
    vm->stack[vm->sp - 1] = a;
    break;
}
