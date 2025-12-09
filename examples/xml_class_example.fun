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

// Example using the stdlib XML class wrapper

include <io/xml.fun>

xml = XML()
doc = xml.from_file("./examples/data/example.xml")
print("doc handle:")
print(doc)
if (doc == 0)
  print("Failed to load XML file")
else
  root = xml.root(doc)
  print("root name:")
  print(xml.name(root))
  print("root text:")
  print(xml.text(root))
