# Adapted for use with project 'ipv6calc' by Peter Bieringer <pb (at) bieringer.de>
# $Id: Makefile,v 1.2 2002/02/25 21:19:02 peter Exp $

all: getopt1.o getopt.o

getopt1.o: getopt1.c
	${CC} ${CFLAGS} -fPIC -o getopt1.o -c -s getopt1.c

getopt.o: getopt.c
	${CC} ${CFLAGS} -fPIC -o getopt.o -c -s getopt.c

clean:
	rm -f *.o *.so.1 *.a
