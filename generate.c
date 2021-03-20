#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#include "arg.h"
#include "util.h"

/* Macros */
#define $(VARNAME) /* TODO: implement something getting variable */ #VARNAME
#define BUFFER_SIZE 64 * 1024

/* Types */
typedef struct {
	char *data;
	size_t len;
} String;

typedef struct {
	String name;
	String value;
} Variable;

/* Prototypes */
static ssize_t newline(String string);
static void usage(void);

/* Globals */
char *argv0;

/* Functions */
static ssize_t
Strline(String string, String *out)
{
	char *tmpptr = string.data;
	while ((tmpptr - string.data) < string.len) {
		if (*(++tmpptr) == '\n') break;
	}
	string.len = tmpptr - string.data;
	*out = string;
	return (tmpptr - string.data);
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
	char input_buf[BUFFER_SIZE], idata[BUFFER_SIZE], *idata_ptr;
	idata_ptr = idata;
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

	/* Initially, readinput points to beginning of all input */
	readinput.data = input.data;
	readinput.len = input.len;

	/* Parsing ("preprocessing") input for variables */
	if ((rb = read(inputfd, idata, BUFFER_SIZE)) < 0)
		die("read (input):");

	readinput.data = idata;
	readinput.len  = rb;

	fprintf(stderr, "=== INPUT: ===\n%s\n==============\n", idata);

	while (Strline(readinput, &parseinput)) {
		readinput.data += parseinput.len + 1;
		readinput.len -= parseinput.len;
	}

	/* Closing an input */
	close(inputfd);

	return 0;
}
