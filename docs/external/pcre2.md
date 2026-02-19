# PCRE2 (Perl-Compatible Regex) extension (optional)

- CMake option: FUN_WITH_PCRE2=ON
- Purpose: Advanced regular expressions via PCRE2.
- Homepage: https://www.pcre.org/

## Opcodes:

- OP_PCRE2_TEST: pops flags, text, pattern; pushes 1/0
- OP_PCRE2_MATCH: pops flags, text, pattern; pushes match map or Nil
- OP_PCRE2_FINDALL: pops flags, text, pattern; pushes array of match maps

## Notes:

- Requires PCRE2 development headers/libs.
- Flags are backend-specific; see implementation for supported bits.
