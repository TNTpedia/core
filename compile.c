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
static void preprocess(int outputfd, int inputfd, String *output);
static void usage(void);

/* Globals */
char *argv0;

static Variable vs[VS_MAX]; /* Variable stack */
static size_t vss; /* Variable stack size */
static char buffer[8192]; /* Buffer for persistent things like variables */
static char *bufptr; /* Pointer to usable space in buffer */

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
preprocess(int outputfd, int inputfd, String *output)
{
	String parseinput, readinput;
	char idata[BUFFER_SIZE];
	ssize_t rb;

	/* Reading data */
	if ((rb = read(inputfd, idata, BUFFER_SIZE)) < 0)
		die("read (input):");

	/* Setting readinput variables */
	readinput.data = idata;
	readinput.len  = rb;

	/* Zeroing first byte of an output data */
	*(output->data) = '\0';

	while (Strtok(readinput, &parseinput, '\n') > 0) {
		if (*(parseinput.data) == '@' && *(parseinput.data + 1) != '@') {
			++parseinput.data;
			if (*(parseinput.data) == '#'); /* comment */
			else if (*(parseinput.data) == '<') { /* do something special */
				++parseinput.data;
				if (!strncmp(parseinput.data, "###", 3)) {
					String function;
					strncat((*output).data, "%",
							MIN(BUFFER_SIZE - ((*output).len)++, 1));
					function = $(function);
					strncat((*output).data, function.data,
							MIN(BUFFER_SIZE - (*output).len, function.len));
					(*output).len += function.len;
					strncat((*output).data, "();%",
							MIN(BUFFER_SIZE - ((*output).len)++, 4));
					(*output).len += 3;
				} else {
					write(outputfd, "\n", 1);
					write(outputfd, parseinput.data, parseinput.len - 2);
				}
				--parseinput.data;
			} else { /* variable */
				String tok;
				if (Strtok(parseinput, &tok, '=') <= 0) {
					/* TODO: return syntax error */;
				}
				strncpy(bufptr, (idata + (parseinput.data - idata)),
						MIN(tok.len - 1, 8192 - (bufptr - buffer)));
				vs[vss].name.data = bufptr;
				vs[vss].name.len = tok.len - 1;
				bufptr += tok.len - 1;
				strncpy(bufptr, (idata + (parseinput.data - idata)) + (tok.len + 1),
						MIN(parseinput.len - (tok.len + 1) - 1, 8192 - (bufptr - buffer)));
				vs[vss].value.data = bufptr;
				vs[vss].value.len = parseinput.len - (tok.len + 1) - 1;
				bufptr += parseinput.len - (tok.len + 1) - 1;
				vs[vss].name = Strtrim(vs[vss].name);
				vs[vss].value = Strtrim(vs[vss].value);
				++vss;
			}
			--parseinput.data;
		} else {
			strncat((*output).data, readinput.data,
					MIN(BUFFER_SIZE - (*output).len, parseinput.len));
			(*output).len += parseinput.len;
		}
		readinput.data += parseinput.len;
		readinput.len -= parseinput.len;
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
	size_t vsscopy, viter;
	char tname[sizeof TEMPLATEDIR + 256];
	char input_buf[BUFFER_SIZE];
	int fd;
	String function = $(function);
	String fun_iden = Striden(templatename);
	String input = {
		.data = input_buf,
		.len = 0
	};

	/* Saving stack size */
	vsscopy = vss;
	/* Declaring function name as templatename */
	/* DECLVAR_S(function, fun_iden); TODO: recursive templates handling */

	strcpy(tname, TEMPLATEDIR);
	strncpy(tname + (sizeof TEMPLATEDIR - 1), templatename.data, MIN(templatename.len + 1, 256));

	/* Opening template */
	if ((fd = open(tname, O_RDONLY)) < 0)
		die("open (template '%s'):", tname);

	/* Preprocessing */
	preprocess(outputfd, fd, &input);

	write(outputfd, "void ", 5);
	write(outputfd, fun_iden.data, fun_iden.len);
	write(outputfd, "(void) {\n", 9);

	/* Declaring variables */
	for (viter = vsscopy; viter < vss; ++viter)
		dprintf(outputfd, "DECLVAR(%.*s, \"%.*s\"); ",
				vs[viter].name.len,  vs[viter].name.data,
				vs[viter].value.len, vs[viter].value.data);

	/* Generating C code */
	generateC(outputfd, input);
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

	/* Input data */
	char input_buf[BUFFER_SIZE];

	String input = {
		.data = input_buf,
		.len = 0
	}, caller;

	/* Options parsing */
	ARGBEGIN {
	case 'o':
		outputfn = ARGF(); break;
	case 'v':
		die("compile from stacinhtml-"VERSION); break;
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
	bufptr = buffer;

	/* Declaring a few variables */
	DECLVAR(title, outputfn);
	DECLVAR(template, "basic.stac");

	/* Opening an input */
	if ((inputfd = open(inputfn, O_RDONLY)) < 0)
		die("open (input):");

	/* Opening an output */
	if ((outputfd = open(outputfn, O_WRONLY | O_CREAT, 0644)) < 0)
		die("open (output):");

	/* Actual preprocessing */
	preprocess(outputfd, inputfd, &input);

	/* Closing an input */
	close(inputfd);

	/* Writing beginning of content() */
	write(outputfd, "#include <assemble.h>\nvoid content(void) {", 42);

	/* And after that, generating C code to output */
	generateC(outputfd, input);

	/* After this, closing content() */
	write(outputfd, "\n}", 2);

	/* And finally adding templates */
	DECLVAR(function, "content");
	template(outputfd, getVariableByName("template")->value);
	STACKPOP();

	/* main() beginning */
	write(outputfd, "\nint main(void) {\n\t", 19);

	/* Declaring variables */
	for (viter = 0; viter < vss; ++viter)
		dprintf(outputfd, "DECLVAR(%.*s, \"%.*s\"); ",
				vs[viter].name.len,  vs[viter].name.data,
				vs[viter].value.len, vs[viter].value.data);

	/* calling function */
	caller = Striden($(template));
	write(outputfd, caller.data, caller.len);

	/* main() ending */
	write(outputfd, "();\n}\n", 6);

	/* Closing an output */
	close(outputfd);

	return 0;
}
