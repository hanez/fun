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

// Minimal XML example using libxml2-backed builtins

doc = xml_parse("<root><item id=\"1\">a</item><item id=\"2\">b</item></root>")
print("doc handle=\(doc)")
root = xml_root(doc)
print("root name=\(xml_name(root)) text=\(xml_text(root))")
