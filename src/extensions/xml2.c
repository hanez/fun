/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file xml2.c
 * @brief Lightweight libxml2 handle registry for Fun VM extension helpers.
 *
 * This module provides small fixed-size registries for libxml2 objects when
 * compiled with FUN_WITH_XML2. Documents and nodes can be associated with
 * small integer handles to make them easier to pass around inside the
 * interpreter and its C API boundaries. The document registry owns the
 * underlying xmlDoc, while the node registry does not own the xmlNode — nodes
 * are freed when their owning document is freed.
 *
 * Limits: The registries are fixed-size (docs: 64, nodes: 256). Handle value 0
 * is reserved and indicates failure/invalid.
 *
 * Thread-safety: Not thread-safe. Coordinate access externally if needed.
 */
#ifdef FUN_WITH_XML2
#include <libxml/parser.h>
#include <libxml/tree.h>

/**
 * @brief Slot describing a registered XML document.
 *
 * The registry owns the xmlDocPtr and will free it when the handle is
 * released via xml_doc_free_handle().
 */
typedef struct {
  xmlDocPtr doc;
  int in_use;
} XmlDocSlot;
/**
 * @brief Slot describing a registered XML node.
 *
 * The registry does not own the node; it is managed by its document. Releasing
 * a node handle does not free the node memory.
 */
typedef struct {
  xmlNodePtr node;
  int in_use;
} XmlNodeSlot;

static XmlDocSlot g_xml_docs[64];
static XmlNodeSlot g_xml_nodes[256];

/**
 * @brief Allocate a document handle for the given xmlDoc pointer.
 *
 * @param d Valid xmlDocPtr to register. Ownership is transferred to the
 *          registry, which will xmlFreeDoc() it when the handle is freed.
 * @return Positive handle in the range [1, 63] on success; 0 if no slot is
 *         available.
 */
static int xml_doc_alloc(xmlDocPtr d) {
  for (int i = 1; i < (int)(sizeof(g_xml_docs) / sizeof(g_xml_docs[0])); ++i) {
    if (!g_xml_docs[i].in_use) {
      g_xml_docs[i].in_use = 1;
      g_xml_docs[i].doc = d;
      return i;
    }
  }
  return 0;
}
/**
 * @brief Retrieve a registered xmlDoc by handle.
 *
 * @param h Handle previously returned by xml_doc_alloc().
 * @return xmlDocPtr if the handle is valid and in use; NULL otherwise.
 */
static xmlDocPtr xml_doc_get(int h) {
  if (h > 0 && h < (int)(sizeof(g_xml_docs) / sizeof(g_xml_docs[0])) && g_xml_docs[h].in_use) return g_xml_docs[h].doc;
  return NULL;
}
/**
 * @brief Free a document handle and the underlying xmlDoc.
 *
 * @param h Handle to release.
 * @return 1 if the handle was valid and has been released; 0 otherwise.
 */
static int xml_doc_free_handle(int h) {
  if (h <= 0 || h >= (int)(sizeof(g_xml_docs) / sizeof(g_xml_docs[0])) || !g_xml_docs[h].in_use) return 0;
  if (g_xml_docs[h].doc) xmlFreeDoc(g_xml_docs[h].doc);
  g_xml_docs[h].doc = NULL;
  g_xml_docs[h].in_use = 0;
  return 1;
}

/**
 * @brief Allocate a node handle for the given xmlNode pointer.
 *
 * @param n Valid xmlNodePtr to register. Ownership is NOT transferred;
 *          nodes are managed by their owning document.
 * @return Positive handle in the range [1, 255] on success; 0 if no slot is
 *         available.
 */
static int xml_node_alloc(xmlNodePtr n) {
  for (int i = 1; i < (int)(sizeof(g_xml_nodes) / sizeof(g_xml_nodes[0])); ++i) {
    if (!g_xml_nodes[i].in_use) {
      g_xml_nodes[i].in_use = 1;
      g_xml_nodes[i].node = n;
      return i;
    }
  }
  return 0;
}
/**
 * @brief Retrieve a registered xmlNode by handle.
 *
 * @param h Handle previously returned by xml_node_alloc().
 * @return xmlNodePtr if the handle is valid and in use; NULL otherwise.
 */
static xmlNodePtr xml_node_get(int h) {
  if (h > 0 && h < (int)(sizeof(g_xml_nodes) / sizeof(g_xml_nodes[0])) && g_xml_nodes[h].in_use) return g_xml_nodes[h].node;
  return NULL;
}
/**
 * @brief Free a node handle without freeing the underlying node.
 *
 * Nodes are owned by their document; releasing the document invalidates any
 * associated node handles.
 *
 * @param h Handle to release.
 * @return 1 if the handle was valid and has been released; 0 otherwise.
 */
static int xml_node_free_handle(int h) {
  if (h <= 0 || h >= (int)(sizeof(g_xml_nodes) / sizeof(g_xml_nodes[0])) || !g_xml_nodes[h].in_use) return 0;
  /* nodes are owned by their document; do not free here */
  g_xml_nodes[h].node = NULL;
  g_xml_nodes[h].in_use = 0;
  return 1;
}
#endif
