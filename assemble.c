#include "assemble.h"

/* Globals */
Variable vs[VS_MAX]; /* Variable stack */
size_t vss; /* Variable stack size */

/* Functions */
String
getVariableValue(char *v)
{
	size_t n;
	String s = { .data = v, .len = strlen(v) };
	n = vss;
	while (--n >= 0) {
		if (!Strcmp(vs[n].name, s))
			return vs[n].value;
	}
}
