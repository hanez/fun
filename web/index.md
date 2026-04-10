---
layout: page
title: :load /index.fun
subtitle: The programming language that makes you have fun!
description: Blog and Homepage
metasub: news
noToc: true
noDate: false
tags:
- fun
- project
- dynamic
- static
- programming
- blog
- language
permalink: /
---

<h2>What is Fun?</h2>

<p>Fun is an experiment, just for fun, but Fun works!</p>

<p>Fun is a highly strict programming language, but also highly simple. It looks like Python (My favorite language), but there are differences.</p>

<p>Influenced by <a href="https://www.gnu.org/software/bash/" class="ext">Bash</a>, <a href="https://en.wikipedia.org/wiki/The_C_Programming_Language" class="ext">C</a>, <a href="https://www.lua.org/" class="ext">Lua</a>, PHP, <a href="https://www.python.org/" class="ext">Python</a>, and a little Rust (Most influences came from linked languages).</p>

<p>Fun is and will ever be 100% free under the terms of the <a href="https://opensource.org/license/apache-2-0" class="ext">Apache-2.0 License</a>.</p>

<p style="text-align:right;"><a href="/about/" title="About">[Read more]</a></p>

<!--<p><a href="/about/">More about Fun</a><p>-->

<h2>Blog (<a href="/feed/" title="Blog Feed" class="rss">Feed</a>)</h2>

{% for post in site.tags.pinned %}
<article class="post-content">
<h2><a href="{{ post.url }}" title="{{ post.title }}">{{ post.title }}</a> <span style="cursor:help; float: right;" title="Pinned Post!">📌</span></h2>
<p class="post-meta">{{ post.date | date: "%b %-d, %Y %H:%M:%S" }} ({{ post.date | date: "%b %-d, %Y %H:%M:%S" | date: "%Z" }})
{% if post.date_updated %} - Edited: {{ post.date_updated | date: "%b %-d, %Y %H:%M:%S" }} ({{ post.date_updated | date: "%b %-d, %Y %H:%M:%S" | date: "%Z" }}){% endif %}{% if post.author %}, <a href="{{ site.git_short }}/{{ post.author }}" target="_blank" class="user">{{ post.author }}</a>{% endif %}</p>
{{ post.content | truncatewords: site.truncatewords_length }}
<p style="text-align:right;"><a href="{{ post.url }}" title="{{ post.url }}">[Read post]</a></p>
</article>
{% endfor %}

{% for post in site.posts %}
{% unless post.tags contains 'pinned' %}
<article class="post-content">
<h2><a href="{{ post.url }}" title="{{ post.title }}">{{ post.title }}</a></h2>
<p class="post-meta">{{ post.date | date: "%b %-d, %Y %H:%M:%S" }} ({{ post.date | date: "%b %-d, %Y %H:%M:%S" | date: "%Z" }})
{% if post.date_updated %} - Edited: {{ post.date_updated | date: "%b %-d, %Y %H:%M:%S" }} ({{ post.date_updated | date: "%b %-d, %Y %H:%M:%S" | date: "%Z" }}){% endif %}{% if post.author %}, <a href="{{ site.git_short }}/{{ post.author }}" target="_blank" class="user">{{ post.author }}</a>{% endif %}</p>
{{ post.content | truncatewords: site.truncatewords_length }}
<p style="text-align:right;"><a href="{{ post.url }}" title="{{ post.url }}">[Read post]</a></p>
</article>
{% endunless %}
{% endfor %}

<h2>Pages</h2>

<div id="pages">
{% if paginator.page == 1 %}<a href="/blog/" class="active" title="Page 1">1</a>{% else %}<a href="/blog/" title="Page 1">1</a>{% endif %}
{% for count in (2..paginator.total_pages) %}
{% if count == paginator.page %}<a href="/{{ count }}/" class="active" title="Page {{ count }}">{{ count }}</a>{% else %}<a href="/{{ count }}/" title="Page {{ count }}">{{ count }}</a>{% endif %}
{% endfor %}
</div><br>

