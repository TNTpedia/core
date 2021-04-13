#ifndef _LIBSTAC_H
#define _LIBSTAC_H

#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>

#include "../str.h"
#include "../util.h"

/* Macros */
#define $(VARNAME) getVariableValue(#VARNAME)
#define $$(VARNAME) write(fd, $(VARNAME).data, $(VARNAME).len);
#define DECLVAR(VARNAME, VALUE) \
	(vs[vss].name.len = strlen(vs[vss].name.data = #VARNAME), \
	 vs[vss].value.len = strlen(vs[vss].value.data = VALUE == NULL ? "<null>" : VALUE), \
	 ++vss)
#define fd 1 /* stdout */

#define echo(STRING) write(fd, (STRING), sizeof(STRING) - 1)
#define PERCENT write(fd, "\x25", 1);

ssize_t print(char *what);
ssize_t nprint(char *what, size_t n);
int lsprintf(char *fmt, ...);

#endif
