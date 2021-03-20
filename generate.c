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
#define DECLVAR(VARNAME, VALUE) \
	(vs[vss].name.len = strlen(vs[vss].name.data = #VARNAME), \
	 vs[vss].value.len = strlen(vs[vss].value.data = VALUE), \
	 ++vss)
#define BUFFER_SIZE 64 * 1024
#define VS_MAX 256

/* Types */
typedef struct {
	String name;
	String value;
} Variable;

/* Prototypes */
static String getVariableValue(char *varname);
static void usage(void);

/* Globals */
char *argv0;

Variable vs[VS_MAX]; /* Variable stack */
size_t vss; /* Variable stack size */

/* Functions */
static String
getVariableValue(char *v)
{
	size_t n;
	String s = { .data = v, .len = strlen(v) };
	n = -1;
	while (++n < vss) {
		if (!Strcmp(vs[n].name, s))
			return vs[n].value;
	}
}

static void
usage(void)
{
	die("usage: %s [-v] [-o OUTPUT] INPUT [TEMPLATE]", argv0);
}

/* Main */
int
main(int argc, char *argv[])
{
	/* Variables: */
	/* File names and file descriptors */
	char *inputfn = NULL, *outputfn = NULL, *templatefn = NULL;
	int inputfd, templatefd;

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
	else if (argc == 2)
		templatefn = argv[1];
	else
		usage();

	if (outputfn == NULL) /* TODO */
		die("output not specified (needed in current stage)");

	/* Opening an input */
	if ((inputfd = open(inputfn, O_RDONLY)) < 0)
		die("open:");

	/* Zeroing first byte of input data */
	*(input.data) = '\0';

	/* Setting variable stack size to 0 */
	vss = 0;

	/* Initially, readinput points to beginning of all input */
	readinput.data = input.data;
	readinput.len = input.len;

	/* Declaring a few variables */
	DECLVAR(title, outputfn);
	DECLVAR(template, templatefn == NULL ? "none" : templatefn);

	/* Parsing ("preprocessing") input for variables */
	if ((rb = read(inputfd, idata, BUFFER_SIZE)) < 0)
		die("read (input):");

	readinput.data = idata;
	readinput.len  = rb;

	while (Strtok(readinput, &parseinput, '\n') > 0) {
		if (*(readinput.data) == '@' && *(readinput.data + 1) != '@') {
			++readinput.data;
			if (*(readinput.data) == '#'); /* comment */
			else { /* variable */
				String tok;
				if (Strtok(readinput, &tok, '=') <= 0)
					/* TODO: return syntax error */;
				vs[vss].name.data = (idata + (parseinput.data - idata) + 1);
				vs[vss].name.len = tok.len;
				vs[vss].value.data = (idata + (parseinput.data - idata) + 1) + (tok.len + 1);
				vs[vss].value.len = parseinput.len - (tok.len + 1) - 1;
				vs[vss].name = Strtrim(vs[vss].name);
				vs[vss].value = Strtrim(vs[vss].value);
				++vss;
			}
			--readinput.data;
		} else {
			strncat(input.data, readinput.data, MIN(BUFFER_SIZE - input.len, parseinput.len + 1));
			input.len += parseinput.len;
		}
		readinput.data += parseinput.len + 1;
		readinput.len -= parseinput.len;
	}

	int i;
	for (i = 0; i < vss; ++i)
		printf(" * [%d]: %.*s = \"%.*s\"\n", i,
				vs[i].name.len,  vs[i].name.data,
				vs[i].value.len, vs[i].value.data);

	/* Closing an input */
	close(inputfd);

	return 0;
}
