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
#include <sys/types.h>
#include <string.h>

typedef struct {
	char *data;
	size_t len;
} String;

String toString(char *s);
int Strcmp(String a, String b);
ssize_t Strtok(String string, String *out, char c);
String Striden(String string);
String Strtrim(String str);

#endif
