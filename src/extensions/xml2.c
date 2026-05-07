/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file xml2.c
 * @brief libxml2 handle registries and helper utilities for the Fun VM extension.
 *
 * Overview
 * --------
 * This translation unit implements compact, fixed-size registries that assign
 * small positive integer handles to libxml2 objects. VM opcodes under
 * src/vm/xml2/*.c and higher-level helpers can exchange these integer handles
 * across the VM stack without exposing raw pointers.
 *
 * Build-time feature flag
 * -----------------------
 * Code in this file is compiled only when the CMake option FUN_WITH_XML2 is
 * enabled (i.e., the preprocessor symbol FUN_WITH_XML2 is defined). When
 * disabled, this file contributes no symbols and the corresponding VM opcodes
 * should be compiled out or provide a graceful fallback.
 *
 * Ownership and lifetime
 * ----------------------
 * - Document registry (XmlDocSlot) TAKES OWNERSHIP of the registered xmlDocPtr.
 *   Releasing the document handle will call xmlFreeDoc() for the stored
 *   pointer.
 * - Node registry (XmlNodeSlot) DOES NOT take ownership of xmlNodePtr values.
 *   Nodes are owned by their document. Releasing a node handle only clears the
 *   registry slot; it does not free the underlying xmlNode memory.
 * - When a document is freed, the libxml2 tree below it is destroyed by
 *   libxml2, thereby invalidating any node pointers previously returned from
 *   that document. The small node handle registry in this file does not track
 *   which document owns which node. Callers must ensure they do not use node
 *   handles after their owning document has been released; they should clear
 *   those node handles explicitly if necessary.
 *
 * Limits and handle ranges
 * ------------------------
 * - Document handles are allocated from a fixed-size array of 64 slots. Valid
 *   handles are in the inclusive range [1, 63].
 * - Node handles are allocated from a fixed-size array of 256 slots. Valid
 *   handles are in the inclusive range [1, 255].
 * - The value 0 is reserved and indicates failure or an invalid handle.
 *
 * Error handling
 * --------------
 * - Allocation requests fail with a return value of 0 when no free slot is
 *   available.
 * - Lookups return NULL for invalid or unused handles.
 * - Free functions return 1 when a valid in-use handle was released, and 0
 *   otherwise. Document release also frees the underlying xmlDoc via
 *   xmlFreeDoc(). Node release does not free the xmlNode memory.
 *
 * Thread-safety
 * -------------
 * - The registries are simple, unsynchronized arrays. They are NOT
 *   thread-safe. If the VM uses libxml2 handles across multiple threads,
 *   external synchronization is required around all calls into this module.
 *
 * Example
 * -------
 * @code{.c}
 * // Assume xmlInitParser() was called by the embedding application.
 * xmlDocPtr d = xmlReadMemory("<root><x/></root>", 20, "-", NULL, 0);
 * int dh = xml_doc_alloc(d);   // takes ownership of d
 * xmlDocPtr same = xml_doc_get(dh);
 * // ... use 'same' with libxml2 APIs ...
 *
 * // When done, free the document handle; this calls xmlFreeDoc(same).
 * (void)xml_doc_free_handle(dh);
 * @endcode
 */
#ifdef FUN_WITH_XML2
#include <libxml/parser.h>
#include <libxml/tree.h>

/**
 * @brief Slot describing a registered XML document.
 *
 * The registry owns the xmlDocPtr and will free it when the handle is released
 * via xml_doc_free_handle().
 */
typedef struct {
  xmlDocPtr doc; /**< Pointer to a parsed libxml2 document (owned by registry). */
  int in_use;    /**< Non-zero if the slot is occupied. */
} XmlDocSlot;
/**
 * @brief Slot describing a registered XML node.
 *
 * The registry does not own the node; it is managed by its document. Releasing
 * a node handle does not free the node memory.
 */
typedef struct {
  xmlNodePtr node; /**< Pointer to a node in some document (not owned). */
  int in_use;      /**< Non-zero if the slot is occupied. */
} XmlNodeSlot;

/**
 * @brief Fixed-size registry storage for documents.
 *
 * Index 0 is reserved. Valid document handle indices are in [1, 63].
 */
static XmlDocSlot g_xml_docs[64];
/**
 * @brief Fixed-size registry storage for nodes.
 *
 * Index 0 is reserved. Valid node handle indices are in [1, 255].
 */
static XmlNodeSlot g_xml_nodes[256];

/**
 * @brief Allocate a document handle for the given xmlDoc pointer.
 *
 * Allocates a free slot in the document registry and stores the supplied
 * xmlDocPtr. Ownership is transferred to the registry; releasing the handle
 * will call xmlFreeDoc() for the stored pointer.
 *
 * @param d Valid xmlDocPtr to register. The caller must not free the document
 *          directly after a successful registration.
 * @return Positive handle in the range [1, 63] on success; 0 if no slot is
 *         available.
 *
 * @note Passing NULL is undefined behavior for this helper in the sense that
 *       it will simply not find a slot and return 0 if no slot is free; callers
 *       should validate inputs before calling.
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
 * Performs bounds and state checks on the registry and returns the stored
 * xmlDocPtr for the handle if present.
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
 * If the handle is valid and in use, calls xmlFreeDoc() for the stored
 * document pointer, clears the slot, and returns 1.
 *
 * @param h Handle to release.
 * @return 1 if the handle was valid and has been released; 0 otherwise.
 *
 * @note Freeing a document invalidates any xmlNodePtr previously obtained from
 *       that document. The node registry in this file does not automatically
 *       clear entries that reference nodes from the freed document; callers are
 *       responsible for avoiding use-after-free by releasing or ignoring such
 *       node handles.
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
 * Allocates a free slot in the node registry and stores the supplied pointer.
 * Ownership is not transferred; the memory is managed by the owning document.
 *
 * @param n Valid xmlNodePtr to register. Must remain valid for as long as the
 *          handle is in use and the owning document is alive.
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
 * Performs bounds and state checks on the registry and returns the stored
 * xmlNodePtr for the handle if present.
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
 * Clears the registry slot associated with the given handle. The underlying
 * xmlNode memory is not freed because nodes are owned by their document.
 *
 * @param h Handle to release.
 * @return 1 if the handle was valid and has been released; 0 otherwise.
 *
 * @note Releasing or freeing a document invalidates any nodes originating from
 *       that document. Callers should avoid using node handles after their
 *       document has been released.
 */
static int xml_node_free_handle(int h) {
  if (h <= 0 || h >= (int)(sizeof(g_xml_nodes) / sizeof(g_xml_nodes[0])) || !g_xml_nodes[h].in_use) return 0;
  /* nodes are owned by their document; do not free here */
  g_xml_nodes[h].node = NULL;
  g_xml_nodes[h].in_use = 0;
  return 1;
}
#endif
