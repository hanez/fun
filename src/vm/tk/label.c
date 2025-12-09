/* TK_LABEL */
case OP_TK_LABEL: {
    /* stack: ..., id, text -> rc */
    Value textv = pop_value(vm);
    Value idv = pop_value(vm);
    char *text = value_to_string_alloc(&textv);
    char *id = value_to_string_alloc(&idv);
    free_value(textv);
    free_value(idv);
    if (!id) { if (text) free(text); push_value(vm, make_int(-1)); break; }
    if (!text) { text = strdup(""); }
    /* escape id minimally (dots and word chars are fine) -> just use as-is */
    /* escape text for Tcl double quotes */
    size_t n = 0; for (const char *p = text; *p; ++p) { n += (*p == '\\' || *p == '"') ? 2 : 1; }
    char *et = (char*)malloc(n + 1);
    if (!et) { free(id); free(text); push_value(vm, make_int(-1)); break; }
    char *q = et; for (const char *p = text; *p; ++p) { if (*p == '\\' || *p == '"') *q++ = '\\'; *q++ = *p; } *q = '\0';
    free(text);
    size_t slen = strlen(id) + strlen(et) + 128;
    char *script = (char*)malloc(slen);
    if (!script) { free(id); free(et); push_value(vm, make_int(-1)); break; }
    snprintf(script, slen,
             "if {[winfo exists .%s]} { .%s configure -text \"%s\" } else { label .%s -text \"%s\" }",
             id, id, et, id, et);
    int rc = fun_tk_eval_script(script);
    free(script);
    free(id);
    free(et);
    push_value(vm, make_int(rc));
    break;
}
