# Adapted for use with project 'ipv6calc' by Peter Bieringer <pb (at) bieringer.de>
# $Id: Makefile,v 1.3 2005/09/16 09:07:36 peter Exp $

CC = gcc

all: md5.o

md5.o: md5.c
	${CC} ${CFLAGS} -fPIC -o md5.o -c -s md5.c

clean:
	rm -f *.o *.so.1 *.a

distclean:
	# nothing to do
