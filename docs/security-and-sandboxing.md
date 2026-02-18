# Security and Sandboxing

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
