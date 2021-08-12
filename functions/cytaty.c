#ifndef _CYTATY_C
#define _CYTATY_C

#include <functions/libwiki.c>

static int cssincluded = 0;

#define cytat(CYTAT, ...) { \
	lsprintf("<blockquote class=\"cytat\"><i>%s</i> ", CYTAT); \
	__VA_ARGS__; \
	echo("</blockquote>"); \
}

#endif
