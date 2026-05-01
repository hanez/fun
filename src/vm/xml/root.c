/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file root.c
 * @brief VM opcode snippet for retrieving the root node of an XML document.
 *
 * Included by vm.c; implements OP_XML_ROOT.
 *
 * Opcode: OP_XML_ROOT
 * - Stack effect: pop (int doc_handle) → push (int node_handle | 0)
 * - Behavior: For a valid document handle, obtains the document's root
 *   element and returns a positive node handle. If the document is invalid
 *   or has no root element, 0 is pushed.
 * - Gating: If FUN_WITH_XML2 is disabled, the input is discarded and 0 is
 *   pushed.
 *
 * Notes
 * - Returned node handles are managed by the XML node registry. They must be
 *   released by corresponding XML VM opcodes to avoid leaks.
 *
 * Example
 *  - stack: [ doc_handle ]
 *  - OP_XML_ROOT → [ node_handle ] or [ 0 ]
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
  Value drop = pop_value(vm);
  free_value(drop);
  push_value(vm, make_int(0));
#endif
  break;
}
