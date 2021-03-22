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

INDIR = in/
INDIRS = $(sort $(shell find $(INDIR)/ -type d))
IN = $(sort $(shell find $(INDIR)/ -type f -name '*.stac'))

METADIR = meta/
METADIRS = $(subst $(INDIR),$(METADIR),$(INDIRS))
META = $(subst $(INDIR),$(METADIR),$(IN:.stac=.c))
BIN = $(META:.c=.bin)

OUTDIR = out/
OUTDIRS = $(subst $(METADIR),$(OUTDIR),$(METADIRS))
OUT = $(subst $(METADIR),$(OUTDIR),$(BIN:.bin=.html))

GENERATOR = ./generate

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
