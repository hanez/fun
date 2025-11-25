/**
 * libcurl POST builtin
 */
case OP_CURL_POST: {
#ifdef FUN_WITH_CURL
    Value vbody = pop_value(vm);
    Value vurl  = pop_value(vm);
    char *url  = value_to_string_alloc(&vurl);
    char *body = value_to_string_alloc(&vbody);
    free_value(vurl);
    free_value(vbody);
    if (!url) { if (body) free(body); push_value(vm, make_string("")); break; }
    if (!body) body = strdup("");
    FunCurlBuf buf = { NULL, 0 };
    CURL *h = curl_easy_init();
    if (!h) { free(url); free(body); push_value(vm, make_string("")); break; }
    curl_easy_setopt(h, CURLOPT_URL, url);
    curl_easy_setopt(h, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(h, CURLOPT_POST, 1L);
    curl_easy_setopt(h, CURLOPT_POSTFIELDS, body);
    curl_easy_setopt(h, CURLOPT_WRITEFUNCTION, fun_curl_write_cb);
    curl_easy_setopt(h, CURLOPT_WRITEDATA, &buf);
    CURLcode rc = curl_easy_perform(h);
    curl_easy_cleanup(h);
    free(url);
    free(body);
    if (rc != CURLE_OK) {
        if (buf.d) free(buf.d);
        push_value(vm, make_string(""));
        break;
    }
    Value s = make_string(buf.d ? buf.d : "");
    if (buf.d) free(buf.d);
    push_value(vm, s);
#else
    Value a = pop_value(vm); free_value(a);
    Value b = pop_value(vm); free_value(b);
    push_value(vm, make_string(""));
#endif
    break;
}
