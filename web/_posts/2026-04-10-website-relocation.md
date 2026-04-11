---
layout: post
published: true
author: hanez
author_email: you@hanez.org
author_url: https://hanez.org
noToc: false
title: Moving the Website to the Project Root
description: Relocating the fun-lang.xyz source code for easier maintenance.
date: 2026-04-10
categories:
- news
- project
tags:
- website
- meta
- maintenance
- jekyll
---

We've recently completed a significant change to our project structure: the source code for the [fun-lang.xyz](https://fun-lang.xyz){:class="ext"} website has been moved directly into the root of the main Fun project repository.

### Why the Move?

The main reason for merging this code into the root of the Fun project is that it is much easier for me to maintain the whole project this way. 

Having everything in one place simplifies our development workflow:

- **Unified Versioning:** All project components, including the website and its documentation, now share the same version control history.
- **Simplified Maintenance:** Updating documentation and the website together ensures that the online presence stays in sync with the language's development.
- **Streamlined Workflow:** It's now possible to update the language, its libraries, the documentation, and the blog in a single commit if needed.

### Where is it located?

You can now find the website source in the `./web/` directory of the repository. It continues to use the [Jekyll](https://jekyllrb.com/){:class="ext"} static site generator.

For those interested in the technical details or looking to contribute to the website, we have added a new documentation file: [/web/documentation/website/](../documentation/website/){:class="git"}. This document covers the technology stack, the directory structure, and the available `Makefile` targets for local development and deployment.

### What changes for you?

For most users, nothing changes! The website remains available at [https://fun-lang.xyz](https://fun-lang.xyz){:class="ext"}. If you are a contributor or following the project closely, you'll just need to look into the `./web/` directory instead of a separate repository.

Happy hacking!

Johannes Findeisen (hanez) - [hanez@fun-lang.xyz](mailto:hanez@fun-lang.xyz){:class="mail"}
