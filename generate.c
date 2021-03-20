#include <stdio.h>

#include "arg.h"
#include "util.h"

static void usage(void);

char *argv0;

static void
usage(void)
{
	die("usage: %s [-v] INPUT [TEMPLATE]", argv0);
}

int
main(int argc, char *argv[])
{
	/* Variables */
	char *input = NULL, *template = NULL;

	/* Options parsing */
	ARGBEGIN {
	case 'v':
		die("generate-"VERSION); break;
	default:
		  usage(); break;
	} ARGEND

	/* Argument parsing */
	if (argc == 1)
		input = argv[0];
	else if (argc == 2)
		template = argv[1];
	else
		usage();

	return 0;
}
