# == Section 0: Includes and basic dependencies ==

include config.mk

all: pages
pages: generator

.PHONY: all pages generator

# == Section 1: Building a generator ==

SRCLIB = util.c
OBJLIB = ${SRCLIB:.c=.o}

SRC = generate.c
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

clean:
	rm -f ${OBJLIB} ${EXE}

.PHONY: clean

# == Section 2: Generating pages ==

INDIR = in
INDIRS = $(sort $(shell find $(INDIR)/ -type d))
IN = $(sort $(shell find $(INDIR)/ -type f -name '*.stac'))

METADIR = meta
METADIRS = $(subst $(INDIR),$(METADIR),$(INDIRS))
META = $(subst $(INDIR),$(METADIR),$(IN:.stac=.c))
BIN = $(META:.c=.bin)

OUTDIR = out
OUTDIRS = $(subst $(METADIR),$(OUTDIR),$(METADIRS))
OUT = $(subst $(METADIR),$(OUTDIR),$(META:.bin=.html))

GENERATOR = ./generate

pages: generator ${OUT} ${GENERATOR}

${OUT}: ${META}
${META}: mkpagedirs

mkpagedirs: ${IN}
	mkdir -p $(OUTDIRS) $(METADIRS)

${META}: ${IN}
	${GENERATOR} -o $@ $(subst $(METADIR),$(INDIR),$(@:.c=.stac))

%.bin: %.c
	${CC} -o $@ $(@:.bin=.c)

${OUT}: ${BIN}
	${GENERATOR} -o $@ $(subst $(OUTDIR),$(BINDIR),$(@:.html=.bin))

.PHONY: mkpagedirs
