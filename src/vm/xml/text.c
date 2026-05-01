/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file text.c
 * @brief VM opcode snippet for retrieving the concatenated text of an XML node.
 *
 * Included by vm.c; implements OP_XML_TEXT.
 *
 * Opcode: OP_XML_TEXT
 * - Stack effect: pop (int node_handle) → push (string text)
 * - Behavior: Retrieves the node's textual content as produced by
 *   xmlNodeGetContent(), which concatenates all descendant text nodes.
 *   If the node is invalid or has no textual content, an empty string is
 *   pushed.
 * - Gating: If FUN_WITH_XML2 is disabled, the input is discarded and an empty
 *   string is pushed.
 *
 * Notes
 * - The returned string is copied into a VM string value; libxml2 buffers are
 *   freed immediately after use.
 *
 * Example
 *  // stack: [ node_handle ]
 *  OP_XML_TEXT   // → [ "Hello world" ]
 */
/* OP_XML_TEXT: pops node handle; pushes string (concatenate text node children) */
case OP_XML_TEXT: {
#ifdef FUN_WITH_XML2
  Value vh = pop_value(vm);
  int h = (vh.type == VAL_INT) ? (int)vh.i : 0;
  xmlNodePtr n = xml_node_get(h);
  free_value(vh);
  if (!n) {
    push_value(vm, make_string(""));
    break;
  }
  xmlChar *content = xmlNodeGetContent(n);
  if (!content) {
    push_value(vm, make_string(""));
    break;
  }
  push_value(vm, make_string((const char *)content));
  xmlFree(content);
#else
  Value drop = pop_value(vm);
  free_value(drop);
  push_value(vm, make_string(""));
#endif
  break;
}
