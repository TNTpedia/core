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
