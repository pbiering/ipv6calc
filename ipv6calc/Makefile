# Project    : ipv6calc/ipv6calc
# File       : Makefile
# Version    : $Id: Makefile,v 1.1 2002/03/18 20:00:20 peter Exp $
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

ipv6calc:	$(OBJS)
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
