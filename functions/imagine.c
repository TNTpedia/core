#ifndef _IMAGINE_C
#define _IMAGINE_C

#include <functions/libwiki.c>

static int cssincluded = 0;

void
imagine_begin(void)
{
	if (!cssincluded) {
		lsprintf("<link rel=\"stylesheet\" href=\"/data/imagine.css?shasum=");
		cssincluded = 1;
	}
	system("sha1sum /var/www/tntpedia/html/data/imagine.css");
	lsprintf("\" />");

	lsprintf("\t<div class=\"imagine\">");
}

void
imagine_end(void)
{
	lsprintf("\t</div>");
}

#define imagine(URL, ...) { \
	imagine_begin(); \
	lsprintf("<img src=\"/data/static/%s\" />", URL); \
	__VA_ARGS__; \
	imagine_end(); \
}

#endif
