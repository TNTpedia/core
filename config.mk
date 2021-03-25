# basics
MAJORVERSION = 0
SUBVERSION   = 0
PATCHLEVEL   = 1
BUILDNAME    = vanilla

VERSION = ${MAJORVERSION}.${SUBVERSION}.${PATCHLEVEL}-${BUILDNAME}

# directories
INDIR = in/
METADIR = meta/
OUTDIR = out/
TEMPLATEDIR = templates/

# flags
CFLAGS   = -std=c99 -Wall -Wextra -pedantic -Ofast
CPPFLAGS = -D_XOPEN_SOURCE=700 -DVERSION=\"${VERSION}\" -DTEMPLATEDIR=\"${TEMPLATEDIR}\"
LDFLAGS  = -Ofast -static
CCFLAGS  = ${CFLAGS} ${CPPFLAGS}

# compiler
CC = gcc
