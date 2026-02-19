# LibreSSL extension (optional)

- CMake option: FUN_WITH_LIBRESSL=ON
- Purpose: provide small crypto helpers backed by LibreSSL's libcrypto. Includes md5, sha256, sha512, ripemd160 helpers under libressl_* names.
- Homepage: https://www.libressl.org/

## Build notes:

- Requires system LibreSSL development headers and libcrypto. The build detects LibreSSL directly (via pkg-config or standard include/lib locations) and links to libcrypto from LibreSSL — it does not require OpenSSL to be installed.
- When disabled, the builtins below evaluate to empty strings to mirror optionality behavior across extensions.

## Provided helper functions/opcodes:

- Function: libressl_md5(data:string) -> string (lowercase hex).
- Function: libressl_sha256(data:string) -> string (lowercase hex).
- Function: libressl_sha512(data:string) -> string (lowercase hex).
- Function: libressl_ripemd160(data:string) -> string (lowercase hex).
- Opcodes: OP_LIBRESSL_MD5, OP_LIBRESSL_SHA256, OP_LIBRESSL_SHA512, OP_LIBRESSL_RIPEMD160 (internal mappings for the functions above).

## Quickstart:

- Configure: `cmake -S . -B build -DFUN_WITH_LIBRESSL=ON`
- Build: `cmake --build build --target fun`
- Run examples:
  - `./build/fun examples/crypto/libressl_md5.fun`
  - `./build/fun examples/crypto/libressl_sha256.fun`
  - `./build/fun examples/crypto/libressl_sha512.fun`
  - `./build/fun examples/crypto/libressl_ripemd160.fun`

## Example output:

- md5(abc) = 900150983cd24fb0d6963f7d28e17f72
- md5("") = d41d8cd98f00b204e9800998ecf8427e
- sha256(abc) = ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad
- sha256("") = e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
- sha512(abc) = ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f
- sha512("") = cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e
- ripemd160(abc) = 8eb208f7e05d987a9b044a8e98c6b087f15a0bfc
- ripemd160("") = 9c1185a5c5e9fc54612808977ee8f548b2258d31

## Notes:

- The functions accept any Fun value; non-strings are coerced via `to_string` semantics by the VM before hashing.
- On some platforms, RIPEMD-160 may not be available; in that case the helper returns an empty string.
- If your system installs LibreSSL headers under `/usr/include/libressl`, CMake will automatically add this directory to the include path so that `#include <libressl/openssl/...>` resolves correctly.
