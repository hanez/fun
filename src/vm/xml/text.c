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

/* OP_XML_TEXT: pops node handle; pushes string (concatenate text node children) */
case OP_XML_TEXT: {
#ifdef FUN_WITH_XML2
    Value vh = pop_value(vm);
    int h = (vh.type == VAL_INT) ? (int)vh.i : 0;
    xmlNodePtr n = xml_node_get(h);
    free_value(vh);
    if (!n) { push_value(vm, make_string("")); break; }
    xmlChar *content = xmlNodeGetContent(n);
    if (!content) { push_value(vm, make_string("")); break; }
    push_value(vm, make_string((const char*)content));
    xmlFree(content);
#else
    Value drop = pop_value(vm); free_value(drop);
    push_value(vm, make_string(""));
#endif
    break;
}
