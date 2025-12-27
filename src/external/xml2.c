/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-12-11 (2025-12-11 migrated from src/vm/libsql/common.c)
 */

#ifdef FUN_WITH_XML2
#include <libxml/parser.h>
#include <libxml/tree.h>

typedef struct { xmlDocPtr doc; int in_use; } XmlDocSlot;
typedef struct { xmlNodePtr node; int in_use; } XmlNodeSlot;

static XmlDocSlot g_xml_docs[64];
static XmlNodeSlot g_xml_nodes[256];

static int xml_doc_alloc(xmlDocPtr d) {
  for (int i = 1; i < (int)(sizeof(g_xml_docs)/sizeof(g_xml_docs[0])); ++i) {
    if (!g_xml_docs[i].in_use) { g_xml_docs[i].in_use = 1; g_xml_docs[i].doc = d; return i; }
  }
  return 0;
}
static xmlDocPtr xml_doc_get(int h) {
  if (h > 0 && h < (int)(sizeof(g_xml_docs)/sizeof(g_xml_docs[0])) && g_xml_docs[h].in_use) return g_xml_docs[h].doc;
  return NULL;
}
static int xml_doc_free_handle(int h) {
  if (h <= 0 || h >= (int)(sizeof(g_xml_docs)/sizeof(g_xml_docs[0])) || !g_xml_docs[h].in_use) return 0;
  if (g_xml_docs[h].doc) xmlFreeDoc(g_xml_docs[h].doc);
  g_xml_docs[h].doc = NULL;
  g_xml_docs[h].in_use = 0;
  return 1;
}

static int xml_node_alloc(xmlNodePtr n) {
  for (int i = 1; i < (int)(sizeof(g_xml_nodes)/sizeof(g_xml_nodes[0])); ++i) {
    if (!g_xml_nodes[i].in_use) { g_xml_nodes[i].in_use = 1; g_xml_nodes[i].node = n; return i; }
  }
  return 0;
}
static xmlNodePtr xml_node_get(int h) {
  if (h > 0 && h < (int)(sizeof(g_xml_nodes)/sizeof(g_xml_nodes[0])) && g_xml_nodes[h].in_use) return g_xml_nodes[h].node;
  return NULL;
}
static int xml_node_free_handle(int h) {
  if (h <= 0 || h >= (int)(sizeof(g_xml_nodes)/sizeof(g_xml_nodes[0])) || !g_xml_nodes[h].in_use) return 0;
  /* nodes are owned by their document; do not free here */
  g_xml_nodes[h].node = NULL;
  g_xml_nodes[h].in_use = 0;
  return 1;
}
#endif
