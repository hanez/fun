# INI (iniparser) extension (optional)

- CMake option: FUN_WITH_INI=ON
- Purpose: Read/write simple INI configuration files.
- Homepage: https://github.com/ndevilla/iniparser

## Opcodes:

- OP_INI_LOAD: pops path; pushes handle (>0) or 0
- OP_INI_FREE: pops handle; pushes 1/0
- OP_INI_GET_STRING: pops def, key, section, handle; pushes string
- OP_INI_GET_INT: pops def, key, section, handle; pushes int
- OP_INI_GET_DOUBLE: pops def, key, section, handle; pushes float
- OP_INI_GET_BOOL: pops def, key, section, handle; pushes 0/1
- OP_INI_SET: pops value, key, section, handle; pushes 1/0
- OP_INI_UNSET: pops key, section, handle; pushes 1/0
- OP_INI_SAVE: pops path, handle; pushes 1/0

## Notes:

- Requires iniparser development headers/libs.
- When disabled, helpers return neutral values (0/empty strings) like other optional extensions.
