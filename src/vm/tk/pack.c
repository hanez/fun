/* TK_PACK */
case OP_TK_PACK: {
    Value idv = pop_value(vm);
    char *id = value_to_string_alloc(&idv);
    free_value(idv);
    if (!id) { push_value(vm, make_int(-1)); break; }
    size_t slen = strlen(id) + 16;
    char *script = (char*)malloc(slen);
    if (!script) { free(id); push_value(vm, make_int(-1)); break; }
    snprintf(script, slen, "pack .%s", id);
    int rc = fun_tk_eval_script(script);
    free(script);
    free(id);
    push_value(vm, make_int(rc));
    break;
}
