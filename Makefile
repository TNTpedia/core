# == Section 0: Includes ==

include config.mk

# == Section 1: Generating a generator ==

SRCLIB = util.c
OBJLIB = ${SRCLIB:.c=.o}

SRC = generate.c
BIN = ${SRC:.c=}

all: options ${BIN}

options:
	@echo build options:
	@echo "CFLAGS       = ${CFLAGS}"
	@echo "CPPFLAGS     = ${CPPFLAGS}"
	@echo "LDFLAGS      = ${LDFLAGS}"
	@echo "CC           = ${CC}"

${BIN}: ${SRC}

${BIN}: ${OBJLIB}

${OBJLIB}: ${SRCLIB}

%.o: %.c
	${CC} ${CCFLAGS} -c -o $@ $<

%: %.c ${OBJLIB}
	${CC} ${CCFLAGS} ${LDFLAGS} -o $@ $^

clean:
	rm -f ${OBJLIB} ${BIN}

.PHONY: all clean

# == Section 2: Generating pages ==
