#!/usr/bin/env fun

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

/*
 * Access a namespaced XML file and show prefixed element names.
 */

include <io/xml.fun>

path = "./examples/data/ns_example.xml"
xml = XML()
doc = xml.from_file(path)
if (doc == 0)
  print("Failed to load: ")
  print(path)
else
  root = xml.root(doc)
  // With namespaces, the node name may include the prefix, e.g., "ns:library"
  print("Root element: ")
  print(xml.name(root))

  content = read_file(path)
  // Extract first book title and author (namespace prefix bk:)
  b1 = xml.between(content, "<bk:book", "</bk:book>")
  title = xml.between(b1, "<bk:title>", "</bk:title>")
  author = xml.between(b1, "<bk:author>", "</bk:author>")
  print("First book title: ")
  print(title)
  print("Author: ")
  print(author)

/* Expected output:
Root element: 
<?xml version="1.0" encoding="UTF-8"?>
<ns:library xmlns:ns="http://example.org/ns/library" xmlns:bk="http://example.org/ns/book">
  <bk:book id="B-1">
    <bk:title>The Art of Fun</bk:title>
    <bk:author>J. Findeisen</bk:author>
  </bk:book>
  <bk:book id="B-2">
    <bk:title>Minimal VM Design</bk:title>
    <bk:author>A. Dev</bk:author>
  </bk:book>
</ns:library>

First book title: 
The Art of Fun
Author: 
J. Findeisen
*/
