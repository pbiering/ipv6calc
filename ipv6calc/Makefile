# Project    : ipv6calc/ipv6calc
# File       : Makefile
# Version    : $Id: Makefile,v 1.4 2002/03/24 17:30:24 peter Exp $
# Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
#
# Information:
#  Makefile for ipv6calc
#

# Compiler and Linker Options
COPTS = -O2 -Wall -g

CFLAGS	= $(COPTS) -I../getopt/ -I../ -I../lib/

CC	= gcc

OBJS	= ipv6calc.o showinfo.o

all:		ipv6calc

.c.o:
		$(CC) $(CFLAGS) -c $<

$(OBJS):	ipv6calc.h showinfo.h

libipv6calc.a:	
		cd ../ && make lib-make

ipv6calc:	$(OBJS) libipv6calc.a
		echo "Compile: ipv6calc"
		$(CC) -o ipv6calc $(OBJS) $(GETOBJS) -lipv6calc -L../lib/ -lcrypto

static:		ipv6calc
		$(CC) -o ipv6calc-static $(OBJS) $(GETOBJS) -lcrypto -static

distclean:
		make clean

clean:
		rm -f ipv6calc *.o

test:
		make
		./test_ipv6calc.sh || exit 1
