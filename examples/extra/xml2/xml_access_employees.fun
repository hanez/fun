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
 * Access selected fields in employees.xml using minimal XML API.
 */

include <io/xml.fun>

path = "./examples/data/employees.xml"
xml = XML()
doc = xml.from_file(path)
if (doc == 0)
  print("Failed to load: ")
  print(path)
else
  root = xml.root(doc)
  print("Root element: ")
  print(xml.name(root))

  content = read_file(path)
  // Find the first <employee> block and extract its fields
  first_emp = xml.between(content, "<employee", "</employee>")
  name = xml.between(first_emp, "<name>", "</name>")
  role = xml.between(first_emp, "<role>", "</role>")
  email = xml.between(first_emp, "<email>", "</email>")
  emp_id = xml.between(first_emp, "id=\"", "\"")

  print("First employee id: ")
  print(emp_id)
  print("Name: ")
  print(name)
  print("Role: ")
  print(role)
  print("Email: ")
  print(email)

/* Expected output:
Root element: 
<?xml version="1.0" encoding="UTF-8"?>
<company>
  <department name="Engineering">
    <employee id="E-100">
      <name>Alice Doe</name>
      <role>Senior Developer</role>
      <email>alice@example.com</email>
    </employee>
    <employee id="E-101">
      <name>Bob Roe</name>
      <role>DevOps Engineer</role>
      <email>bob@example.com</email>
    </employee>
  </department>
  <department name="Sales">
    <employee id="S-200">
      <name>Carol Smith</name>
      <role>Account Executive</role>
      <email>carol@example.com</email>
    </employee>
  </department>
</company>

First employee id: 
E-100
Name: 
Alice Doe
Role: 
Senior Developer
Email: 
alice@example.com
*/
