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
./stac -t templates/basic.stac in/$ARTICLE.stac
