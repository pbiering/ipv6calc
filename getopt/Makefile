# Adapted for use with project 'ipv6calc' by Peter Bieringer <pb (at) bieringer.de>
# $Id: Makefile,v 1.3 2005/07/11 15:04:05 peter Exp $

CC = gcc

all: getopt1.o getopt.o

getopt1.o: getopt1.c
	${CC} ${CFLAGS} -fPIC -o getopt1.o -c -s getopt1.c

getopt.o: getopt.c
	${CC} ${CFLAGS} -fPIC -o getopt.o -c -s getopt.c

clean:
	rm -f *.o *.so.1 *.a
