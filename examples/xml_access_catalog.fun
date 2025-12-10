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
 * Demonstrate reading specific fields from an XML file using
 * the minimal XML API (root name) and simple string parsing.
 */

include <io/xml.fun>

path = "./examples/data/catalog.xml"
xml = XML()
doc = xml.from_file(path)
if (doc == 0)
  print("Failed to load: ")
  print(path)
else
  root = xml.root(doc)
  print("Root element: ")
  print(xml.name(root))

  // Show how to access specific fields by quick-and-dirty parsing
  content = read_file(path)  // raw XML text
  // First product name
  name = xml.between(content, "<name>", "</name>")
  // First price value and its currency attribute (extract value, then attribute)
  price_val = xml.between(content, "<price", "</price>")
  currency = ""
  if (len(price_val) > 0)
    currency = xml.between(price_val, "currency=\"", "\"")
    // strip attribute tag part
    price_text = xml.between(price_val, ">", "")  // until end
    if (len(price_text) == 0)
      price_text = price_val
  else
    price_text = ""

  print("First product name: ")
  print(name)
  print("First price: ")
  if (len(currency) > 0)
    print(currency)
    print(" ")
  print(price_text)

/* Expected output:
Root element: 
<?xml version="1.0" encoding="UTF-8"?>
<catalog>
  <product id="SKU-1001">
    <name>Wireless Keyboard</name>
    <category>Peripherals</category>
    <price currency="USD">39.99</price>
    <specs>
      <layout>US</layout>
      <connection>Bluetooth</connection>
      <battery>AA</battery>
    </specs>
  </product>
  <product id="SKU-2002">
    <name>27" Monitor</name>
    <category>Displays</category>
    <price currency="USD">199.00</price>
    <specs>
      <resolution>2560x1440</resolution>
      <panel>IPS</panel>
      <refresh>75Hz</refresh>
    </specs>
  </product>
  <product id="SKU-3003">
    <name>USB-C Dock</name>
    <category>Peripherals</category>
    <price currency="USD">89.50</price>
    <specs>
      <ports>2xHDMI, 3xUSB-A, 1xUSB-C PD</ports>
      <pd>65W</pd>
    </specs>
  </product>
</catalog>

First product name: 
Wireless Keyboard
First price: 
USD
 
39.99
*/
