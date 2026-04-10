---
layout: page
published: true
noToc: true
noComments: false
noDate: false
title: Fun - Website Documentation (fun-lang.xyz)
subtitle: Documentation for the [fun-lang.xyz](https,//fun-lang.xyz) website in the `./web/` directory.
description: Documentation for the [fun-lang.xyz](https,//fun-lang.xyz) website in the `./web/` directory.
permalink: /documentation/website/
lang: en
tags:
- documentation
- handbook
- installation
- usage
- introduction
- help
- guide
- howto
- docs
- specifications
- specs
- repl
---

# Website Documentation (fun-lang.xyz)

This document describes the structure and maintenance of the website for the Fun programming language, located in the `./web/` directory. The website is hosted at [https://fun-lang.xyz](https://fun-lang.xyz).

## Technology Stack

The website is built using [Jekyll](https://jekyllrb.com/), a static site generator written in Ruby.

- **Markdown Engine:** kramdown (configured for GFM)
- **Syntax Highlighting:** Rouge
- **Plugins:** 
  - `jekyll-paginate`
  - `jekyll-sitemap`
  - `jekyll-toc`
  - `jekyll-seo-tag` (available in Gemfile)

## Directory Structure

The `./web/` directory follows the standard Jekyll structure:

- `_config.yml`: Main configuration file for Jekyll.
- `_data/`: YAML/JSON data files used by the site.
- `_includes/`: Reusable HTML snippets (headers, footers, etc.).
- `_layouts/`: Page templates (e.g., `page`, `post`).
- `_posts/`: Blog posts and news updates.
- `_sass/`: SCSS files for styling.
- `_site/`: The generated static website (usually excluded from version control).
- `assets/`: Images, JavaScript, and CSS (if not in `_sass`).
- `css/`, `js/`, `images/`, `fonts/`: Static assets.
- `about/`, `community/`, `faq/`, `documentation/`, etc.: Static pages and subdirectories.
- `Gemfile`: Ruby dependencies for building the site.

## Makefile Targets

The `./web/` directory contains a `Makefile` to simplify common tasks:

- `make build`: Builds the static site using Jekyll (`bundle exec jekyll build --incremental`).
- `make clean`: Removes the generated `_site/` directory content.
- `make run`: Runs the Jekyll development server with drafts and incremental build enabled.
- `make runreload`: Same as `run`, but with live reload enabled.
- `make minify`: Minifies HTML, CSS, and XML files in the `_site/` directory using the `minify` tool.
- `make sync`: Deploys the `_site/` directory to the production server using `rsync`.
- `make release`: A full release cycle: `build`, `minify`, and `sync`.
- `make local`: Performs a `clean`, `build`, and `minify` for local testing.

## Local Development

To run the website locally for development:

1.  **Prerequisites:** Ensure you have Ruby, Bundler, and optionally the `minify` tool installed.
2.  **Install dependencies:**
    ```bash
    cd web/
    bundle install
    ```
3.  **Run the development server:**
    Using the Makefile:
    ```bash
    make run
    ```
    Or with live reload:
    ```bash
    make runreload
    ```
    The website will be available at `http://localhost:4000`.

## Content Management

- **Blog Posts:** Add new `.md` files to `_posts/` following the `YYYY-MM-DD-title.md` naming convention.
- **Pages:** Create new `.md` or `.html` files in the root of the `./web/` directory or in subdirectories.
- **Configuration:** Update `_config.yml` for site-wide settings, navigation, or social media links.

## Deployment

The website can be deployed using the Makefile:

```bash
cd web/
make release
```

The `release` target builds the site, minifies the assets, and syncs the files to [fun-lang.xyz](https://fun-lang.xyz) via `rsync`. Ensure you have the necessary SSH permissions for the sync to succeed.
