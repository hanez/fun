/* TK_BUTTON */
case OP_TK_BUTTON: {
    /* stack: ..., id, text -> rc */
    Value textv = pop_value(vm);
    Value idv = pop_value(vm);
    char *text = value_to_string_alloc(&textv);
    char *id = value_to_string_alloc(&idv);
    free_value(textv);
    free_value(idv);
    if (!id) { if (text) free(text); push_value(vm, make_int(-1)); break; }
    if (!text) { text = strdup(""); }
    size_t n = 0; for (const char *p = text; *p; ++p) { n += (*p == '\\' || *p == '"') ? 2 : 1; }
    char *et = (char*)malloc(n + 1);
    if (!et) { free(id); free(text); push_value(vm, make_int(-1)); break; }
    char *q = et; for (const char *p = text; *p; ++p) { if (*p == '\\' || *p == '"') *q++ = '\\'; *q++ = *p; } *q = '\0';
    free(text);
    size_t slen = strlen(id) + strlen(et) + 196;
    char *script = (char*)malloc(slen);
    if (!script) { free(id); free(et); push_value(vm, make_int(-1)); break; }
    /*
     * Default behavior: clicking the button should terminate the app. We set
     * -command {catch {destroy .}; exit 0} to both destroy the window and exit
     * the process. Using 'catch' makes it safe if the window is already gone.
     */
    snprintf(script, slen,
             "if {[winfo exists .%s]} { .%s configure -text \"%s\" -command {catch {destroy .}; exit 0} } else { button .%s -text \"%s\" -command {catch {destroy .}; exit 0} }",
             id, id, et, id, et);
    int rc = fun_tk_eval_script(script);
    free(script);
    free(id);
    free(et);
    push_value(vm, make_int(rc));
    break;
}
