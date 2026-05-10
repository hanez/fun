---
layout: post
published: true
author: hanez
author_email: hanez@fun-lang.xyz
author_url: https://hanez.org
noToc: false
title: What is Fun?
description: Fun is a serious, focused programming language and VM aimed at professional environments — used for serial and smartcard communication and, potentially, TCP/Unix sockets.
date: 2026-05-10
date_updated: 2026-05-11
categories:
- project
- language
- vision
tags:
- fun
- language
- serial
- smartcard
- sockets
- vm
---

Fun is a programming language and virtual machine I originally started for fun - but I take it seriously and build it for professional environments. It is not an esoteric language. The current focus areas are:

- Potential TCP/Unix socket communication
- CGI and web server support
- Serial communication
- Smartcard communication

This post gives you a concise picture of what Fun is (and is not), why it exists, and where it is going.

### What Fun is

- A compact language with a small, pragmatic core
- Backed by a lightweight VM designed for reliability
- Geared toward IO-centric tasks where predictability matters
- Built with maintainability in mind, aiming for clear semantics and straightforward tooling

### What Fun is not

- Not an esoteric toy - while the project began "for fun", it has serious goals and standards
- Not a kitchen-sink runtime - we prefer a focused, composable core over bloat

### Primary use cases (today and near-term)

- Fast prototyping of small, low-level tools
- Explorations around TCP and Unix domain sockets for simple networked services
- Talking to devices over serial interfaces (industrial/embedded scenarios)
- Smartcard interactions where exact byte-level control is required

### Design values

- Focus: solve concrete communication problems well
- Simplicity: keep concepts minimal and understandable
- Reliability: deterministic behavior over clever shortcuts
- Portability: aim to run in constrained and varied environments

### Project status

Fun is evolving quickly. Some components are production-ready, others are still experimental. The direction is clear: practical features that make day-to-day, low-level communication tasks easier and safer.

### Get involved

- [Browse features](https://fun-lang.xyz/features/)
- [Read the documentation](https://fun-lang.xyz/documentation/)
- Try examples from the repository and site
- Send feedback and ideas - they directly shape priorities

If you work with serial devices, smartcards, or lightweight networked tools and want a focused, no-nonsense environment, Fun might fit you well. It started as a project for fun - and it stays fun - but the goals are serious.

Happy hacking!

Johannes Findeisen (hanez) - hanez@fun-lang.xyz
