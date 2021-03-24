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

#define _XOPEN_SOURCE 700

#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "arg.h"
#include "str.h"
#include "util.h"

/* Macros */
#define $(VARNAME) getVariableValue(#VARNAME)
#define $$(VARNAME) write(fd, $(VARNAME).data, $(VARNAME).len);
#define DECLVAR(VARNAME, VALUE) \
	(vs[vss].name.len = strlen(vs[vss].name.data = #VARNAME), \
	 vs[vss].value.len = strlen(vs[vss].value.data = VALUE == NULL ? "<null>" : VALUE), \
	 ++vss)
#define DECLVAR_S(VARNAME, VALUE) \
	(vs[vss].name.len = strlen(vs[vss].name.data = #VARNAME), \
	 vs[vss].value = VALUE, \
	 ++vss)
#define DECLVAR_N(VARNAME, VALUE, N) \
	(vs[vss].name.len = strlen(vs[vss].name.data = #VARNAME), \
	 vs[vss].value = VALUE, vs[vss].len = N, \
	 ++vss)
#define STACKPOP() --vss
#define BUFFER_SIZE 64 * 1024
#define VS_MAX 256

/* Types */
typedef struct {
	String name;
	String value;
} Variable;

/* Prototypes */
static void generateCode(int fd, String input, int c_mode);
static void generateC(int fd, String input);
static Variable *getVariableByName(char *v);
static String getVariableValue(char *varname);
static void preprocess(String input, String *output);
static void usage(void);

/* Globals */
char *argv0;

Variable vs[VS_MAX]; /* Variable stack */
size_t vss; /* Variable stack size */

/* Functions */
static void
generateCode(int fd, String input, int c_mode)
{
	if (c_mode) {
		write(fd, input.data, input.len);
	} else {
		size_t i = -1;
		write(fd, "write(fd, \"", 11);
		while (++i < input.len) {
			if (*input.data == '\n')
				dprintf(fd, "\"\n\"");
			dprintf(fd, "\\x%02x", *(input.data++));
		}
		dprintf(fd, "\", %d);", input.len);
	}
}

static void
generateC(int fd, String input)
{
	int c_mode;
	size_t i, li;
	String ibuf;
	c_mode = 0;

	ibuf.data = input.data;
	ibuf.len = input.len;
	for (i = li = 0; i < input.len; ++i) {
		if (input.data[i] == '%') {
			ibuf.len = ++i - li - 1;
			generateCode(fd, ibuf, c_mode);
			ibuf.data = input.data + i;
			ibuf.len = input.len - i;
			c_mode = !c_mode;
			li = i;
		}
	}
	generateCode(fd, ibuf, c_mode);
}

static Variable *
getVariableByName(char *v)
{
	ssize_t n;
	String s = { .data = v, .len = strlen(v) };
	n = vss;
	while (--n >= 0) {
		if (!Strcmp(vs[n].name, s))
			return vs + n;
	}
}

static String
getVariableValue(char *v)
{
	return getVariableByName(v)->value;
}

static void
preprocess(String input, String *output)
{
	String parseinput;
	char *idata = input.data;

	while (Strtok(input, &parseinput, '\n') > 0) {
		if (*(input.data) == '@' && *(input.data + 1) != '@') {
			++input.data;
			if (*(input.data) == '#'); /* comment */
			else { /* variable */
				String tok;
				if (Strtok(input, &tok, '=') <= 0) {
					/* TODO: return syntax error */;
				}
				vs[vss].name.data = (idata + (parseinput.data - idata) + 1);
				vs[vss].name.len = tok.len;
				vs[vss].value.data = (idata + (parseinput.data - idata) + 1) + (tok.len + 1);
				vs[vss].value.len = parseinput.len - (tok.len + 1) - 1;
				vs[vss].name = Strtrim(vs[vss].name);
				vs[vss].value = Strtrim(vs[vss].value);
				++vss;
			}
			--input.data;
		} else {
			strncat((*output).data, input.data, MIN(BUFFER_SIZE - (*output).len, parseinput.len + 1));
			(*output).len += parseinput.len;
		}
		input.data += parseinput.len + 1;
		input.len -= parseinput.len;
	}
}

static void
usage(void)
{
	die("usage: %s [-v] [-o OUTPUT] INPUT [TEMPLATE]", argv0);
}

static void
template(int outputfd, String templatename)
{
	size_t vsscopy;
	char tname[sizeof TEMPLATEDIR + 256];
	int fd;
	String function = $(function);
	String fun_iden = Striden(templatename);

	/* Saving stack size */
	vsscopy = vss;
	/* Declaring function name as templatename */
	DECLVAR_S(function, fun_iden);

	strcpy(tname, TEMPLATEDIR);
	strncpy(tname + (sizeof TEMPLATEDIR - 1), templatename.data, MIN(templatename.len, 256));

	/* Opening template */
	if ((fd = open(tname, O_RDONLY)) < 0)
		die("open (template '%s'):", tname);

	write(outputfd, "void ", 5);
	write(outputfd, fun_iden.data, fun_iden.len);
	write(outputfd, "(void) {\n", 9);
	/* preprocess(readinput, &input); */
	write(outputfd, "}\n", 2);

	/* Restoring stack size */
	vss = vsscopy;

	/* Closing template */
	close(fd);
}

/* Main */
int
main(int argc, char *argv[])
{
	/* Variables: */
	/* File names and file descriptors */
	char *inputfn = NULL, *outputfn = NULL;
	int inputfd, outputfd;
	size_t viter;

	/* Read bytes (from nextline()/write()) */
	ssize_t rb;

	/* Input data */
	char input_buf[BUFFER_SIZE], idata[BUFFER_SIZE];

	String input = {
		.data = input_buf,
		.len = 0
	}, readinput, parseinput;

	/* Options parsing */
	ARGBEGIN {
	case 'o':
		outputfn = ARGF(); break;
	case 'v':
		die("generate-"VERSION); break;
	default:
		  usage(); break;
	} ARGEND

	/* Argument parsing */
	if (argc == 1)
		inputfn = argv[0];
	else
		usage();

	if (outputfn == NULL) /* TODO */
		die("output not specified (needed in current stage)");

	/* Setting variable stack size to 0 */
	vss = 0;

	/* Declaring a few variables */
	DECLVAR(title, outputfn);
	DECLVAR(template, "basic.stac");

	/* Opening an input */
	if ((inputfd = open(inputfn, O_RDONLY)) < 0)
		die("open (input):");

	/* Zeroing first byte of input data */
	*(input.data) = '\0';

	/* Initially, readinput points to beginning of all input */
	readinput.data = input.data;
	readinput.len = input.len;

	/* Reading data */
	if ((rb = read(inputfd, idata, BUFFER_SIZE)) < 0)
		die("read (input):");

	readinput.data = idata;
	readinput.len  = rb;

	/* Actual preprocessing */
	preprocess(readinput, &input);

	/* Closing an input */
	close(inputfd);

	/* Opening an output */
	if ((outputfd = open(outputfn, O_WRONLY | O_CREAT, 0644)) < 0)
		die("open (output):");

	/* Writing beginning of content() */
	write(outputfd, "#include <assemble.h>\nvoid content(void) {", 42);

	/* Declaring variables */
	for (viter = 0; viter < vss; ++viter)
		dprintf(outputfd, "DECLVAR(%.*s, \"%.*s\"); ",
				vs[viter].name.len,  vs[viter].name.data,
				vs[viter].value.len, vs[viter].value.data);

	/* And after that, generating C code to output */
	generateC(outputfd, input);

	/* After this, closing content() */
	write(outputfd, "\n}", 2);

	/* And finally adding templates */
	DECLVAR(function, "content");
	template(outputfd, getVariableByName("template")->value);
	STACKPOP();

	write(outputfd, "\nint main(void) {\n\t", 19);
	/* We assign that to parseinput, because it is variable
	   that is unused in this place, so we will not create
	   another temporary variable */
	parseinput = Striden($(template));
	write(outputfd, parseinput.data, parseinput.len);
	write(outputfd, "();\n}\n", 6);

	/* Closing an output */
	close(outputfd);

	return 0;
}
