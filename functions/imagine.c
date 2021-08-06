#ifndef _IMAGINE_C
#define _IMAGINE_C

#include <functions/libwiki.c>

void
imagine(char *url, char *about)
{
	lsprintf("<link rel=\"stylesheet\" href=\"/data/imagine.css?shasum=");
	system("sha1sum /var/www/tntpedia/html/data/imagine.css");
	lsprintf("\" />");

	lsprintf("\t<div class=\"imagine\"><img src=\"/data/static/%s\" />", url);
	if (about != NULL) {
		lsprintf("%s", about);
	}
	lsprintf("\t</div>");
}

#endif
