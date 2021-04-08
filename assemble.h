/*
   stacinhtml - STAtic C IN HTML - simple static site generator
   Copyright (C) 2021  Kacper Kocot <kocotian@kocotian.pl>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

*/

#ifndef _ASSEMBLE_H
#define _ASSEMBLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "libstac/libstac.h"
#include "str.h"
#include "util.h"

/* Macros */
#define VS_MAX 256

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
