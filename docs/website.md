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

## Local Development

To run the website locally for development:

1.  **Prerequisites:** Ensure you have Ruby and Bundler installed.
2.  **Install dependencies:**
    ```bash
    cd web/
    bundle install
    ```
3.  **Run the development server:**
    ```bash
    bundle exec jekyll serve
    ```
    The website will be available at `http://localhost:4000`.

## Content Management

- **Blog Posts:** Add new `.md` files to `_posts/` following the `YYYY-MM-DD-title.md` naming convention.
- **Pages:** Create new `.md` or `.html` files in the root of the `./web/` directory or in subdirectories.
- **Configuration:** Update `_config.yml` for site-wide settings, navigation, or social media links.

## Deployment

The website is automatically built and deployed to [fun-lang.xyz](https://fun-lang.xyz) (details depend on the hosting environment, but typically involve running `jekyll build` and uploading the `_site/` directory).
