# basics
MAJORVERSION = 0
SUBVERSION   = 0
PATCHLEVEL   = 1
BUILDNAME    = vanilla

VERSION = ${MAJORVERSION}.${SUBVERSION}.${PATCHLEVEL}-${BUILDNAME}

# flags
CFLAGS   = -std=c99 -Wall -Wextra -pedantic -Ofast
CPPFLAGS = -D_XOPEN_SOURCE=700 -DVERSION=\"${VERSION}\"
LDFLAGS  = -Ofast -static
CCFLAGS  = ${CFLAGS} ${CPPFLAGS}

# compiler
CC = cc
