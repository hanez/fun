---
layout: page
published: true
noToc: false
noComments: false
noDate: false
title: Fun - Security and Sandboxing
subtitle: Trust boundaries, I/O expectations, and capability restrictions.
description: Trust boundaries, I/O expectations, and capability restrictions.
permalink: /documentation/security-and-sandboxing/
lang: en
tags:
- boundaries
- capability
- expectations
- restrictions
- sandboxing
- security
- trust
---

Understand the trust boundaries and how to run Fun code safely.

## Trust model

- By default, Fun code can access functionality exposed by the stdlib and any enabled extensions.
- File and network access depend on available modules and host configuration.

## Running untrusted code

- Prefer running in a container/VM with restricted filesystem and network.
- Limit available stdlib/modules by controlling `FUN_LIB_DIR` contents.
- Use OS-level sandboxing (seccomp, AppArmor, SELinux, chroot) where applicable.

## Best practices

- Avoid running as root.
- Validate and sanitize inputs at module boundaries.
- Keep your build minimal; disable unneeded extensions at compile time.
