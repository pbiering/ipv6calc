# Project    : ipv6calc/lib
# File       : Makefile
# Version    : $Id: Makefile,v 1.4 2002/03/24 17:30:44 peter Exp $
# Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
#
# Information:
#  Makefile for ipv6calc library
#

# Compiler and Linker Options
COPTS = -O2 -Wall

CFLAGS	= $(COPTS) -I../

CC	= gcc
AR	= ar
RANLIB	= ranlib

OBJS	= libipv6calc.o librfc1924.o librfc1884.o libmac.o libeui64.o librfc3041.o libipv6addr.o librfc2874.o librfc1886.o librfc3056.o libipv4addr.o libifinet6.o libieee.o ipv6calchelp.o ipv6calctypes.o

all:		libipv6calc.a

.c.o:
		cd ../ && make db-ieee-oui-make
		cd ../ && make db-ipv4-assignment-make
		$(CC) $(CFLAGS) -c $<

$(OBJS):	libipv6calcdebug.h libipv6addr.h librfc1884.h librfc2874.h libeui64.h libmac.h librfc3041.h libipv6addr.h librfc1924.h librfc1886.h librfc3056.h libipv4addr.h libifinet6.h libieee.h ipv6calchelp.h ipv6calctypes.h

libipv6calc.a:	$(OBJS)
		cd ../ && make db-ieee-oui-make
		cd ../ && make db-ipv4-assignment-make
		echo "Create library"
		$(AR) rs libipv6calc.a $(OBJS)
		$(RANLIB) libipv6calc.a

distclean:
		make clean
		rm -f libieeeoui.h

autoclean:
		make distclean

clean:
		rm -f *.o *.a

install:
		echo "Nothing to install here"
