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

SRCLIB = util.c
OBJLIB = ${SRCLIB:.c=.o}

SRC = compile.c
EXE = ${SRC:.c=}

generator: buildoptions ${EXE} assemble.o

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

INDIRS = $(sort $(shell find $(INDIR)/ -type d))
IN = $(sort $(shell find $(INDIR)/ -type f -name '*.stac'))

METADIRS = $(subst $(INDIR),$(METADIR),$(INDIRS))
META = $(subst $(INDIR),$(METADIR),$(IN:.stac=.c))
BIN = $(META:.c=.bin)

OUTDIRS = $(subst $(METADIR),$(OUTDIR),$(METADIRS))
OUT = $(subst $(METADIR),$(OUTDIR),$(BIN:.bin=.html))

GENERATOR = ./compile

pages: ${OUT}

${OUT}: ${META}
${META}: ${IN}
${IN}: mkpagedirs

mkpagedirs:
	mkdir -p $(OUTDIRS) $(METADIRS)

${IN}: generator ${GENERATOR}

${METADIR}/%.c: ${INDIR}/%.stac
	${GENERATOR} -o $@ $(subst $(METADIR),$(INDIR),$(@:.c=.stac))

${METADIR}/%.bin: ${METADIR}/%.c
	${CC} -o $@ -I. $(@:.bin=.c) assemble.o

${OUTDIR}/%.html: ${METADIR}/%.bin
	./$(subst $(OUTDIR),$(METADIR),$(@:.html=.bin)) > $@

clean:
	rm -f ${OBJLIB} ${EXE} *.o
	rm -rf ${METADIR} ${OUTDIR}

.PHONY: mkpagedirs clean
