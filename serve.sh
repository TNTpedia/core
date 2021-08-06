#!/bin/sh

NAME=$(basename $DOCUMENT_URI)
ARTICLE=$(echo $NAME | sed 's/^./\u&/g')
echo "$NAME" | grep -q '^[A-Z]' || {
	echo "Location: /wiki/$ARTICLE\r\n"
	exit
}

echo -n "Content-type: text/html\r\n"
echo -n "Server: nginx/stac\r\n"
echo

stat "in/$ARTICLE.stac" > /dev/null 2>&1 || ARTICLE="Specjalna:404"
echo > "meta/$ARTICLE.c"
./compile -t templates/basic.stac -o "meta/$ARTICLE.c" in/$ARTICLE.stac || ARTICLE="Specjalna:500"
echo > "meta/$ARTICLE.bin"
gcc -o meta/$ARTICLE.bin -I. meta/$ARTICLE.c util.o assemble.o str.o libstac/libstac.o 2>&1 || ARTICLE="Specjalna:500"
meta/$ARTICLE.bin 2>&1 || ARTICLE="Specjalna:500"
# cat "$DOCUMENT_ROOT$DOCUMENT_URI.html"
