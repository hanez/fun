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

/* OP_XML_ROOT: pops doc handle; pushes node handle (>0) or 0 */
case OP_XML_ROOT: {
#ifdef FUN_WITH_XML2
    Value vh = pop_value(vm);
    int h = (vh.type == VAL_INT) ? (int)vh.i : 0;
    xmlDocPtr doc = xml_doc_get(h);
    free_value(vh);
    int nh = 0;
    if (doc) {
        xmlNodePtr root = xmlDocGetRootElement(doc);
        if (root) nh = xml_node_alloc(root);
    }
    push_value(vm, make_int(nh));
#else
    Value drop = pop_value(vm); free_value(drop);
    push_value(vm, make_int(0));
#endif
    break;
}
