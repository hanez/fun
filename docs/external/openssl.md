# OpenSSL extension (optional)

- CMake option: FUN_WITH_OPENSSL=ON
- Purpose: provide small crypto helpers backed by OpenSSL. Includes md5, sha256, sha512, ripemd160 helpers.
- Homepage: https://www.openssl.org/

## Build notes:

- Requires system OpenSSL development headers and libraries.
- On OpenSSL 3.x, legacy MD5_* APIs are deprecated; you may see warnings during build.

## Provided helper/opcodes:

- Function: openssl_md5(data:string) -> string (lowercase hex). Falls back to empty string when the extension is disabled, mirroring other optional modules.
- Function: openssl_sha256(data:string) -> string (lowercase hex).
- Function: openssl_sha512(data:string) -> string (lowercase hex).
- Function: openssl_ripemd160(data:string) -> string (lowercase hex). Note: On OpenSSL 3.x this may require the legacy provider; if the digest is unavailable, the helper returns an empty string.
- Opcodes: OP_OPENSSL_MD5, OP_OPENSSL_SHA256, OP_OPENSSL_SHA512, OP_OPENSSL_RIPEMD160 (internal mappings for the functions above).

## Quickstart:

- Configure: cmake -S . -B build -DFUN_WITH_OPENSSL=ON
- Build: cmake --build build --target fun
- Run examples:
  - ./build/fun examples/crypto/openssl_md5.fun
  - ./build/fun examples/crypto/openssl_sha256.fun
  - ./build/fun examples/crypto/openssl_sha512.fun
  - ./build/fun examples/crypto/openssl_ripemd160.fun

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

- The OpenSSL 3.x provider configuration on your system determines availability of RIPEMD-160. If the legacy provider is not enabled, openssl_ripemd160() will return an empty string.
