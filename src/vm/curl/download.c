/**
 * libcurl DOWNLOAD builtin
 */
case OP_CURL_DOWNLOAD: {
#ifdef FUN_WITH_CURL
    Value vpath = pop_value(vm);
    Value vurl  = pop_value(vm);
    char *url  = value_to_string_alloc(&vurl);
    char *path = value_to_string_alloc(&vpath);
    free_value(vurl);
    free_value(vpath);
    if (!url || !path) {
        if (url) free(url);
        if (path) free(path);
        push_value(vm, make_int(0));
        break;
    }
    FILE *fp = fopen(path, "wb");
    if (!fp) {
        free(url); free(path);
        push_value(vm, make_int(0));
        break;
    }
    CURL *h = curl_easy_init();
    if (!h) {
        fclose(fp);
        free(url); free(path);
        push_value(vm, make_int(0));
        break;
    }
    curl_easy_setopt(h, CURLOPT_URL, url);
    curl_easy_setopt(h, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(h, CURLOPT_WRITEFUNCTION, fun_curl_file_write_cb);
    curl_easy_setopt(h, CURLOPT_WRITEDATA, fp);
    CURLcode rc = curl_easy_perform(h);
    curl_easy_cleanup(h);
    fclose(fp);
    free(url); free(path);
    if (rc != CURLE_OK) { push_value(vm, make_int(0)); break; }
    push_value(vm, make_int(1));
#else
    Value a = pop_value(vm); free_value(a);
    Value b = pop_value(vm); free_value(b);
    push_value(vm, make_int(0));
#endif
    break;
}
