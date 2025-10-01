case OP_INPUT_LINE: {
    /* operand: 0 = no prompt; 1 = has prompt (string or any value convertible to string) */
    int has_prompt = inst.operand ? 1 : 0;
    if (has_prompt) {
        /* pop prompt value and print without newline */
        Value pv = pop_value(vm);
        char *pstr = value_to_string_alloc(&pv);
        if (pstr) {
            fputs(pstr, stdout);
            fflush(stdout);
            free(pstr);
        }
        free_value(pv);
    }

    /* read a line from stdin, dynamically grow buffer */
    size_t cap = 128;
    size_t len = 0;
    char *buf = (char*)malloc(cap);
    if (!buf) {
        fprintf(stderr, "Runtime error: out of memory reading input");
        push_value(vm, make_string(""));
        break;
    }

    int ch;
    while ((ch = fgetc(stdin)) != EOF) {
        if (ch == '\r') {
            /* Handle CRLF by consuming optional following '\n' */
            int next = fgetc(stdin);
            if (next != EOF && next != '\n') {
                ungetc(next, stdin);
            }
            break;
        }
        if (ch == '\n') {
            break;
        }
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = (char*)realloc(buf, cap);
            if (!nb) {
                free(buf);
                fprintf(stderr, "Runtime error: out of memory reading input");
                push_value(vm, make_string(""));
                goto push_done;
            }
            buf = nb;
        }
        buf[len++] = (char)ch;
    }

    /* null-terminate */
    if (len + 1 >= cap) {
        char *nb = (char*)realloc(buf, len + 1);
        if (!nb) {
            free(buf);
            fprintf(stderr, "Runtime error: out of memory finalizing input");
            push_value(vm, make_string(""));
            goto push_done;
        }
        buf = nb;
    }
    buf[len] = '\0';

    /* push as Fun string */
    push_value(vm, make_string(buf));
    free(buf);

push_done:
    break;
}
