#ifndef _ASSEMBLE_H
#define _ASSEMBLE_H

#include <unistd.h>
#include <string.h>

#include "str.h"

/* Macros */
#define $(VARNAME) getVariableValue(#VARNAME)
#define DECLVAR(VARNAME, VALUE) \
	(vs[vss].name.len = strlen(vs[vss].name.data = #VARNAME), \
	 vs[vss].value.len = strlen(vs[vss].value.data = VALUE), \
	 ++vss)
#define VS_MAX 256
#define fd 1 /* stdout */

/* Types */
typedef struct {
	String name;
	String value;
} Variable;

/* Prototypes */
String getVariableValue(char *varname);

/* Externs */
extern Variable vs[VS_MAX]; /* Variable stack */
extern size_t vss; /* Variable stack size */

#endif
