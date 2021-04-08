# basics
MAJORVERSION = 0
SUBVERSION   = 2
PATCHLEVEL   = 0
BUILDNAME    = vanilla

VERSION = ${MAJORVERSION}.${SUBVERSION}.${PATCHLEVEL}-${BUILDNAME}

# directories (KEEP TRAILING SLASH!)
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
