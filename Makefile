SRC = src/fetchy.c
CC ?= cc
CFLAGS = -O2 -std=c99 -Wall -Wextra
LDFLAGS = -lpthread
DEBUGFLAGS = -g -Og -std=c99 -Wall -Wextra
PREFIX ?= /usr/local

all: fetchy

fetchy: ${SRC} src/config.h src/colour.h
	${CC} ${CFLAGS} ${SRC} ${LDFLAGS} -o fetchy

debug:
	${CC}  ${DEBUGFLAGS} ${SRC} ${LDFLAGS} -o fetchy-debug

clean:
	rm -rf fetchy fetchy.dSYM fetchy-debug fetchy-debug.dSYM

install: fetchy
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp fetchy ${DESTDIR}${PREFIX}/bin
	chmod 711 ${DESTDIR}${PREFIX}/bin/fetchy

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/fetchy

.PHONY: all clean debug install uninstall
