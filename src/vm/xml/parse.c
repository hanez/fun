/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-12-09
 */

/* OP_XML_PARSE: pops text string; pushes doc handle (>0) or 0 */
case OP_XML_PARSE: {
#ifdef FUN_WITH_XML2
    static int xml_inited = 0;
    if (!xml_inited) { xmlInitParser(); xml_inited = 1; }
    Value vtext = pop_value(vm);
    char *text = value_to_string_alloc(&vtext);
    free_value(vtext);
    if (!text) { push_value(vm, make_int(0)); break; }
    xmlDocPtr doc = xmlReadMemory(text, (int)strlen(text), NULL, NULL, XML_PARSE_NONET);
    free(text);
    int h = 0;
    if (doc) {
        h = xml_doc_alloc(doc);
        if (!h) { xmlFreeDoc(doc); }
    }
    push_value(vm, make_int(h));
#else
    Value drop = pop_value(vm); free_value(drop);
    push_value(vm, make_int(0));
#endif
    break;
}
