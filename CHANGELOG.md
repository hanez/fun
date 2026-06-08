# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html) (during 0.x.y development phase).

## [0.42.1] - 2026-06-08
### Fixed
- Small test script issue.  
### Changed
- Documentation and website updates. No runtime code changes.
- README, `make`, and `Doxyfile` updates. No runtime code changes.

## [0.42.0] - 2026-06-03
### Added
- Redis/Valkey extension named `redis` (see `src/extensions/redis.c` and `src/vm/redis/*`).
- Redis example programs: `basic_ping.fun`, `hash_ops.fun`, `kv_set_get.fun`, `list_ops.fun`, and `redis_test.fun`.
- Initial Redis documentation page under `web/documentation/extensions/redis/`.
### Changed
- Build system: Redis extension is not compiled by default; enable it explicitly if needed. No other code changes.
- GitHub workflow tweaks. No code changes.

## [0.41.16] - 2026-05-29
### Added
- `playground/` directory for local-only experiments.
### Changed
- `examples/features.fun` and website features page updates.
- README and API documentation updates. No code changes.
- Documentation and web fixes. No code changes.

## [0.41.15] - 2026-05-26
### Changed
- More internal optimizations.

## [0.41.14] - 2026-05-26
### Changed
- Internal optimizations and helper improvements.

## [0.41.13] - 2026-05-26
### Added
- Basic interactive IRC client example (experimental; known issues remain).

## [0.41.12] - 2026-05-26
### Fixed
- Minor fixes and adjustments for the new IRC library.

## [0.41.11] - 2026-05-26
### Added
- Basic IRC library fully written in Fun (see lib/net/irc.fun) and initial examples.

## [0.41.10] - 2026-05-09
### Added
- Doxygen documentation for kcgi opcodes via inline comments. No code changes.
- Blog post and various content updates. No code changes.
### Changed
- CMake output messages tweaked. No code changes.
- Various content/doc cleanups. No code changes.
### Fixed
- kcgi-related check in scripts/check_op_includes.py.

## [0.41.9] - 2026-05-09
### Added
- Basic CGI support using kcgi.
- New “features” section on the website and navigation entry (web/). No code changes.
### Changed
- Content updates across the website and docs. No code changes.

## [0.41.8] - 2026-05-07
### Fixed
- Bug in the PCSC extension.
### Added
- More Doxygen documentation across extensions. No code changes.

## [0.41.7] - 2026-05-07
### Changed
- Refactored the PCRE2 extension.
### Added
- New regex examples demonstrating the PCRE2 extension.

## [0.41.6] - 2026-05-07
### Changed
- Refactoring and cleanups in VM code.

## [0.41.5] - 2026-05-01
### Added
- Doxygen configuration (Doxyfile) and initial documentation scaffolding (Doxygen.in).
- Optional docs build flag (-DFUN_BUILD_DOCS=ON). No code changes.
- .clang-format and .clang-tidy configurations. No code changes.
### Changed
- Doxygen-related code comments cleaned up. No code changes.
- GitHub CI updates to build Doxygen docs. No code changes.
- Removed sudo from make install dependency.

## [0.41.4] - 2026-04-27
### Changed
- scripts/play.fun now behaves more like scripts/run_examples.sh. No internal logic changes.

## [0.41.3] - 2026-04-27
### Changed
- Opcode cleanup.
- GitHub workflow now checks opcode includes. No code changes.

## [0.41.2] - 2026-04-27
### Fixed
- Threading on Alpine Linux.

## [0.41.1] - 2026-04-27
### Fixed
- Threading on Alpine Linux.

## [0.41.0] - 2026-04-27
### Changed
- Parser: default variable scope in functions is now local (fixes scoping issues).

## [0.40.5] - 2026-04-10
### Removed
- All `libsql` support.
- All `tcltk` support.

## [0.40.0] - 2026-03-25
### Added
- Nested functions support in the Fun parser.
- Higher-order function patterns enabled by nesting.

## [0.39.15] - 2026-03-10
### Added
- Initial GitHub Actions CI workflow.
- Automated execution of examples in CI.
### Changed
- Refactored examples directory structure.
### Fixed
- Line number reporting in error messages.

## [0.30.0] - 2025-11-15
### Added
- `libcurl` (cURL) support for networking.
- `CRC32` and `CRC32C` classes in stdlib.

---
*Note: Dates for older versions are approximate based on repository history.*
