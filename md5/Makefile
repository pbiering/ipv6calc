# Adapted for use with project 'ipv6calc' by Peter Bieringer <pb (at) bieringer.de>
# $Id: Makefile,v 1.4 2005/09/16 11:40:19 peter Exp $

CC = gcc

all: md5.o

md5.o: md5.c
	${CC} ${CFLAGS} -fPIC -o md5.o -c -s md5.c

clean:
	rm -f *.o *.so.1 *.a

distclean:
	make clean

autoclean:
	make distclean
