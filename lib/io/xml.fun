/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-12-09
 */

// XML stdlib abstraction wrapping the xml_* VM builtins (libxml2-backed).
// Minimal API for now: parse, from_file, root, name, text

class XML()
  // Parse XML text into a document handle (>0) or 0 on error.
  fun parse(this, text)
    t = to_string(text)
    return xml_parse(t)

  // Load XML from a file path; returns document handle (>0) or 0.
  fun from_file(this, path)
    p = to_string(path)
    data = read_file(p)
    if (len(data) == 0)
      return 0
    return xml_parse(data)

  // Get root node handle (>0) or 0.
  fun root(this, doc)
    return xml_root(doc)

  // Get node name as string.
  fun name(this, node)
    return xml_name(node)

  // Get node concatenated text content as string.
  fun text(this, node)
    return xml_text(node)

  // Utility: return substring of s between delimiters a and b.
  // - If a is not found, returns "".
  // - If b is an empty string, returns everything after the first occurrence of a.
  // - If b is not found after a, returns "".
  fun between(this, s, a, b)
    i = find(s, a)
    if (i < 0)
      return ""
    i = i + len(a)
    rest = substr(s, i, len(s) - i)
    if (len(b) == 0)
      return rest
    j = find(rest, b)
    if (j < 0)
      return ""
    return substr(rest, 0, j)

