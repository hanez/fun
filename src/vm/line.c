case OP_LINE: {
    /* operand holds the source line number */
    vm->current_line = inst.operand;
    break;
}
