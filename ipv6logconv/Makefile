# Project    : ipv6calc/ipv6logconf
# File       : Makefile
# Version    : $Id: Makefile,v 1.3 2002/04/07 10:55:08 peter Exp $
# Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
#
# Information:
#  Makefile for ipv6logconv
#

# Compiler and Linker Options
COPTS = -O2 -Wall -g

CFLAGS	= $(COPTS) -I../getopt/ -I../ -I../lib/

CC	= gcc

OBJS	= ipv6logconv.o ipv6logconvhelp.o

# splint
SPLINT_OPT_OUTSIDE = -uniondef +matchanyintegral --nullassign +ignoresigns -compdef -usedef -modobserver -formatconst -warnposix -exportlocal
SPLINT_TODO_LATER = -unrecog

all:		ipv6logconv

.c.o:
		$(CC) $(CFLAGS) -c $<

$(OBJS):	ipv6logconvoptions.h ipv6logconvhelp.h

ipv6logconv:	$(OBJS)
		echo "Compile: ipv6logconv"
		$(CC) -o ipv6logconv $(OBJS) $(GETOBJS) -lipv6calc -L../lib/

distclean:
		make clean

clean:
		rm -f ipv6logconv *.o

test:

splint:
		splint *.c ../lib/*.c -I.. -I../lib $(SPLINT_OPT_OUTSIDE) $(SPLINT_TODO_LATER)
