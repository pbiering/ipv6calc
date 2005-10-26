# Project    : ipv6calc/lib
# File       : Makefile
# Version    : $Id: Makefile,v 1.17 2005/10/26 09:36:54 peter Exp $
# Copyright  : 2002-2005 by Peter Bieringer <pb (at) bieringer.de>
#
# Information:
#  Makefile for ipv6calc library
#

# Compiler and Linker Options
#  -D__EXTENSIONS__ is for Solaris & strtok_r
COPTS = -O2 -Wall -D__EXTENSIONS__

CFLAGS	= $(COPTS) -I../ -I../getopt/ -I../md5/

CC	= gcc
AR	= ar
RANLIB	= ranlib

OBJS	= libipv6calc.o librfc1924.o librfc1884.o libmac.o libeui64.o librfc3041.o libipv6addr.o librfc2874.o librfc1886.o librfc3056.o libipv4addr.o libifinet6.o libieee.o ipv6calchelp.o ipv6calctypes.o

all:		libipv6calc.a

libieee.o:	../databases/ieee-oui/dbieee_oui.h libieee.c libieee.h
		cd ../ && make db-ieee-oui-make
		$(CC) $(CFLAGS) -c libieee.c

libipv4addr.o:	../databases/ipv4-assignment/dbipv4addr_assignment.h libipv4addr.c libipv4addr.h
		cd ../ && make db-ipv4-assignment-make
		$(CC) $(CFLAGS) -c libipv4addr.c

libipv6addr.o:	../databases/ipv6-assignment/dbipv6addr_assignment.h libipv6addr.c libipv6addr.h
		cd ../ && make db-ipv6-assignment-make
		$(CC) $(CFLAGS) -c libipv6addr.c

.c.o:
		$(CC) $(CFLAGS) -c $<

$(OBJS):	libipv6calcdebug.h libipv6addr.h librfc1884.h librfc2874.h libeui64.h libmac.h librfc3041.h libipv4addr.h librfc1924.h librfc1886.h librfc3056.h libipv4addr.h libifinet6.h libieee.h ipv6calchelp.h ipv6calctypes.h ipv6calc_inttypes.h ipv6calccommands.h

libipv6calc.a:	$(OBJS)
		cd ../ && make db-ieee-oui-make
		cd ../ && make db-ipv4-assignment-make
		cd ../ && make db-ipv6-assignment-make
		echo "Create library"
		$(AR) r libipv6calc.a $(OBJS)
		$(RANLIB) libipv6calc.a

distclean:
		make clean

autoclean:
		make distclean

clean:
		rm -f *.o *.a

install:

test:
