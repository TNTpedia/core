#include <stdlib.h>
#include <string.h>

#include <functions/uri.c>

char *
_GET(char *left)
{
	char *query_encoded = getenv("QUERY_STRING");
	char *query = uri_decode(query_encoded);
	char *cur, *amp, *tmp;
	char *editable, *ret;

	if (query == NULL || !strlen(query))
		return NULL;

	editable = strcpy(malloc(strlen(query) + 2), query);
	editable[strlen(query)] = '&';
	editable[strlen(query) + 1] = '\0';

	cur = editable;
	do {
		if ((amp = strchr(cur, '&')) != NULL)
			*amp = '\0';
		if (!strncmp(left, cur, MIN(strlen(left), strlen(cur)))) {
			ret = malloc(strlen(cur + strlen(left) + 1) + 1);
			strcpy(ret, cur + strlen(left) + 1);
			free(editable);
			free(query);
			return ret;
		}
		cur = amp != NULL ? amp + 1 : amp;
	} while (cur != NULL && strchr(cur, '&') != NULL);
	free(editable);
	free(query);
	return NULL;
}
