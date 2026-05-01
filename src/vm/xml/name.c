/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file name.c
 * @brief VM opcode snippet for XML node name retrieval.
 *
 * This file is included into the main VM dispatch switch in vm.c and
 * implements the OP_XML_NAME instruction.
 *
 * Opcode: OP_XML_NAME
 * - Stack effect: pop (int node_handle) → push (string name)
 * - Success: pushes the element/node name as a string. If the node has no
 *   name, an empty string is pushed.
 * - Errors/edge cases: if the handle is invalid or the node cannot be
 *   retrieved, an empty string is pushed. The instruction does not throw.
 * - Gating: When FUN_WITH_XML2 is not enabled at build time, the operand is
 *   still popped and an empty string is pushed.
 *
 * Notes
 * - Node handles are small positive integers managed by the XML handle
 *   registry defined in the xml2 extension (see extensions/xml2.c).
 * - The pushed string is a new VM value; ownership is transferred to the VM
 *   stack as usual.
 *
 * Example
 *  // ... stack: [ node_handle ]
 *  OP_XML_NAME  // → [ "book" ]
 */
/* OP_XML_NAME: pops node handle; pushes string */
case OP_XML_NAME: {
#ifdef FUN_WITH_XML2
  Value vh = pop_value(vm);
  int h = (vh.type == VAL_INT) ? (int)vh.i : 0;
  xmlNodePtr n = xml_node_get(h);
  free_value(vh);
  const char *name = (n && n->name) ? (const char *)n->name : "";
  push_value(vm, make_string(name));
#else
  Value drop = pop_value(vm);
  free_value(drop);
  push_value(vm, make_string(""));
#endif
  break;
}
