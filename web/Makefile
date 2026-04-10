all:
	make release

build:
	#wget -O ./handbook/handbook.md https://git.xw3.org/fun/fun/raw/branch/main/docs/handbook.md
	bundle exec jekyll b --incremental

clean:
	rm -rf _site/*

freshrelease:
	make clean
	make release

freshreleaseraw:
	make clean
	make build
	make sync

freshrun:
	make clean
	make build
	make minify
	make run

local:
	make clean
	make build
	make minify

localraw:
	make clean
	make build

localrelease:
	make local
	make minify

minify:
	find ./_site -type f -name "*.html" -exec minify --html-keep-document-tags --html-keep-end-tags -o {} {} \;
	find ./_site -type f -name "*.html" -exec sed -i 's/ \/>/>/g' {} +
	#find ./_site -type f -name "*.html" -exec perl -0777 -pi -e 's/\n/ /g' {} +
	find ./_site -type f -name "*.css" -exec minify --css-precision 0 -o {} {} \;
	find ./_site -type f -name "*.xml" -exec minify -o {} {} \;

release:
	make build
	make minify
	make sync

releaseraw:
	make build
	make sync

run:
	bundle exec jekyll s --incremental --drafts

runclean:
	make clean
	bundle exec jekyll s --incremental --drafts

runcleanreload:
	make clean
	bundle exec jekyll s --incremental --drafts --livereload

runreload:
	bundle exec jekyll s --incremental --drafts --livereload

sync:
	rsync -avr --delete --delete-excluded ./_site/ fun-lang.xyz:/var/www/fun-lang.xyz/www/htdocs/
	#ssh a.xw3.org 'cd /var/www/fun-lang.xyz/www/lib/ && git pull'
	#ssh a.xw3.org 'cd /var/www/fun-lang.xyz/www/htdocs/cgi/bin/ && gcc index.c -o index'

