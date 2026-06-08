---
layout: post
published: true
author: hanez
author_email: you@hanez.org
author_url: https://hanez.org
noToc: false
title: Announcing Fun 0.42.1
description: Maintenance release with documentation updates and a small test script fix
date: 2026-06-09
categories:
- news
- releases
tags:
- fun
- release
- docs
- redis
---

We just released Fun 0.42.1. This is a small, safe maintenance update you can adopt immediately.

### What’s in 0.42.1

- Fixed: Small test script issue.
- Changed: Documentation and website updates. No runtime code changes.
- Changed: README, `make`, and `Doxyfile` updates. No runtime code changes.

If you’re already on 0.42.0, this update has no behavioral impact on your programs.

### Recap: Highlights from 0.42.0

In case you missed the previous release, 0.42.0 introduced the optional Redis/Valkey extension powered by `hiredis`, along with example programs and docs.

- New extension: `redis` (connect, cmd, close)
- Examples: `basic_ping.fun`, `hash_ops.fun`, `kv_set_get.fun`, `list_ops.fun`, `redis_test.fun`
- Docs: see Documentation → Extensions → Redis

To enable the Redis extension when building Fun, pass the CMake option `-DFUN_WITH_REDIS=ON` (ensure `hiredis` is installed and visible to the toolchain). See the build guide for details.

Example (Release build directory shown by your setup):

```
cmake -S . -B build -DFUN_WITH_REDIS=ON
cmake --build build --target fun
```

### Changelog

For the complete list of changes and dates, see the project’s [CHANGELOG.md](https://git.xw3.org/fun/fun/src/branch/main/CHANGELOG.md){:class="git"} in the repository.

### Thank you

Thanks to everyone testing, filing issues, and contributing examples and docs. If you want to help, check out the repository, read the docs, and say hi in the community.

Happy hacking!

Johannes Findeisen (hanez) — hanez@fun-lang.xyz
