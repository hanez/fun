# XML (libxml2) extension (optional)

- CMake option: FUN_WITH_XML2=ON
- Purpose: Minimal XML parsing helpers using libxml2.
- Homepage: http://xmlsoft.org/

Opcodes:
- OP_XML_PARSE: pops text; pushes doc handle (>0) or 0
- OP_XML_ROOT: pops doc handle; pushes node handle (>0) or 0
- OP_XML_NAME: pops node handle; pushes string (node name)
- OP_XML_TEXT: pops node handle; pushes string (concatenated text)

Notes:
- Requires libxml2 development headers/libs.
- On many systems, the include path is `/usr/include/libxml2`.
