/**
 * libcurl GET builtin
 */
case OP_CURL_GET: {
#ifdef FUN_WITH_CURL
    Value vurl = pop_value(vm);
    char *url = value_to_string_alloc(&vurl);
    free_value(vurl);
    if (!url) { push_value(vm, make_string("")); break; }
    FunCurlBuf buf = { NULL, 0 };
    CURL *h = curl_easy_init();
    if (!h) { free(url); push_value(vm, make_string("")); break; }
    curl_easy_setopt(h, CURLOPT_URL, url);
    curl_easy_setopt(h, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(h, CURLOPT_WRITEFUNCTION, fun_curl_write_cb);
    curl_easy_setopt(h, CURLOPT_WRITEDATA, &buf);
    CURLcode rc = curl_easy_perform(h);
    curl_easy_cleanup(h);
    free(url);
    if (rc != CURLE_OK) {
        if (buf.d) free(buf.d);
        push_value(vm, make_string(""));
        break;
    }
    Value s = make_string(buf.d ? buf.d : "");
    if (buf.d) free(buf.d);
    push_value(vm, s);
#else
    Value v = pop_value(vm); free_value(v);
    push_value(vm, make_string(""));
#endif
    break;
}
