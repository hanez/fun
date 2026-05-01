/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file parse.c
 * @brief VM opcode snippet for parsing XML text into a document handle.
 *
 * This snippet is included by vm.c and implements OP_XML_PARSE.
 *
 * Opcode: OP_XML_PARSE
 * - Stack effect: pop (string xml_text) → push (int doc_handle | 0)
 * - Behavior: Parses the given XML string into an in-memory xmlDoc.
 *   On success, a positive document handle is allocated via the XML handle
 *   registry and pushed. On failure (malformed XML, OOM), 0 is pushed.
 * - Initialization: Ensures libxml2 is initialized once (xmlInitParser()).
 * - Gating: If FUN_WITH_XML2 is disabled, the input is discarded and 0 is
 *   pushed.
 *
 * Notes
 * - The handle must later be released via appropriate XML VM opcodes that
 *   free documents; leaking handles will leak memory.
 * - The parser is invoked with XML_PARSE_NONET to disallow network access.
 *
 * Example
 *  - stack: [ "<root/>" ]
 *  - OP_XML_PARSE  → [ 1 ]   (example handle)
 */

/* OP_XML_PARSE: pops text string; pushes doc handle (>0) or 0 */
case OP_XML_PARSE: {
#ifdef FUN_WITH_XML2
  static int xml_inited = 0;
  if (!xml_inited) {
    xmlInitParser();
    xml_inited = 1;
  }
  Value vtext = pop_value(vm);
  char *text = value_to_string_alloc(&vtext);
  free_value(vtext);
  if (!text) {
    push_value(vm, make_int(0));
    break;
  }
  xmlDocPtr doc = xmlReadMemory(text, (int)strlen(text), NULL, NULL, XML_PARSE_NONET);
  free(text);
  int h = 0;
  if (doc) {
    h = xml_doc_alloc(doc);
    if (!h) {
      xmlFreeDoc(doc);
    }
  }
  push_value(vm, make_int(h));
#else
  Value drop = pop_value(vm);
  free_value(drop);
  push_value(vm, make_int(0));
#endif
  break;
}
