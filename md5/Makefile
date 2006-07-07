# Adapted for use with project 'ipv6calc' by Peter Bieringer <pb (at) bieringer.de>
# $Id: Makefile,v 1.6 2006/07/07 13:30:50 peter Exp $

CC = gcc

all: md5.o

md5.o: md5.c
	${CC} ${CFLAGS} -I.. -fPIC -o md5.o -c -s md5.c

clean:
	rm -f *.o *.so.1 *.a

distclean:
	${MAKE} clean

autoclean:
	${MAKE} distclean
