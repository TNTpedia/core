#
# stacinhtml - STAtic C IN HTML - simple static site generator
# Copyright (C) 2021  Kacper Kocot <kocotian@kocotian.pl>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation,
# Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
#

# == Section 0: Includes and basic dependencies ==

include config.mk

all: pages
pages: generator

.PHONY: all pages generator

# == Section 1: Building a generator ==

SRCLIB = util.c assemble.c str.c libstac/libstac.c
OBJLIB = ${SRCLIB:.c=.o}

SRC = compile.c
EXE = ${SRC:.c=}

generator: buildoptions ${EXE}

buildoptions:
	@echo build options:
	@echo "CFLAGS       = ${CFLAGS}"
	@echo "CPPFLAGS     = ${CPPFLAGS}"
	@echo "LDFLAGS      = ${LDFLAGS}"
	@echo "CC           = ${CC}"

${EXE}: ${SRC}

${EXE}: ${OBJLIB}

${OBJLIB}: ${SRCLIB}

%.o: %.c
	${CC} ${CCFLAGS} -c -o $@ $<

%: %.c ${OBJLIB}
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ $^

# == Section 2: Generating pages ==

INDIRS = $(sort $(shell find $(INDIR) -type d))
IN = $(sort $(shell find $(INDIR) -type f -name '*.stac'))

METADIRS = $(patsubst $(INDIR)%,$(METADIR)%,$(INDIRS))
META = $(patsubst $(INDIR)%,$(METADIR)%,$(IN:.stac=.c))
BIN = $(META:.c=.bin)

OUTDIRS = $(patsubst $(METADIR)%,$(OUTDIR)%,$(METADIRS))
OUT = $(patsubst $(METADIR)%,$(OUTDIR)%,$(BIN:.bin=.html))

GENERATOR = ./compile

pages:

pages: ${OUT}

${OUT}: ${OUTDIRS}

${OUTDIRS}: ${METADIRS}
	mkdir -p $(OUTDIRS)

${METADIRS}: ${INDIRS}
	mkdir -p $(METADIRS)

${IN}: generator ${GENERATOR}

${METADIR}%.c: ${INDIR}%.stac
	${GENERATOR} -o $@ $(patsubst $(METADIR)%,$(INDIR)%,$(@:.c=.stac))

${METADIR}%.bin: ${METADIR}%.c
	${CC} -o $@ -I. $(@:.bin=.c) ${OBJLIB}

${OUTDIR}%.html: ${METADIR}%.bin
	./$(patsubst $(OUTDIR)%,$(METADIR)%,$(@:.html=.bin)) > $@

clean:
	rm -f ${OBJLIB} ${EXE} *.o
	rm -rf ${METADIR} ${OUTDIR}

.PHONY: mkpagedirs clean
