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

#ifndef _STR_H
#define _STR_H

#include <ctype.h>

typedef struct {
	char *data;
	size_t len;
} String;

static String toString(char *s);
static int Strcmp(String a, String b);
static ssize_t Strtok(String string, String *out, char c);
static String Strtrim(String str);

static String
toString(char *s)
{
	String ret;
	ret.len = strlen(ret.data = s);
	return ret;
}

static int
Strcmp(String a, String b)
{
	if (a.len != b.len) return -1;
	return strncmp(a.data, b.data, a.len);
}

static ssize_t
Strtok(String string, String *out, char c)
{
	char *tmpptr = string.data;
	while ((tmpptr - string.data) < string.len) {
		if (*(++tmpptr) == c) break;
	}
	string.len = tmpptr - string.data;
	*out = string;
	return (tmpptr - string.data);
}

static String
Strtrim(String str)
{
	int i = 0;
	while (isspace(*(str.data)) && i < str.len) {
		++str.data;
		--str.len;
		++i;
	}
	while (isspace(*(str.data + (str.len - 1))))
		--str.len;
	return str;
}

#endif
