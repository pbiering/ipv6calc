# Adapted for use with project 'ipv6calc' by Peter Bieringer <pb (at) bieringer.de>
# $Id: Makefile,v 1.1 2004/10/30 13:02:53 peter Exp $

all: md5.o

md5.o: md5.c
	${CC} ${CFLAGS} -fPIC -o md5.o -c -s md5.c

clean:
	rm -f *.o *.so.1 *.a
