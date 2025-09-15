case OP_POP: {
    if (vm->sp < 0) {
        fprintf(stderr, "Runtime error: stack underflow for POP\n");
        exit(1);
    }
    Value v = pop_value(vm);
    free_value(v); // free the popped value
    break;
}
