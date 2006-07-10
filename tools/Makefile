# Project    : ipv6calc/ipv6calc
# File       : Makefile[.in]
# Version    : $Id: Makefile,v 1.2 2006/07/10 11:42:29 peter Exp $
# Copyright  : 2006 by Peter Bieringer <pb (at) bieringer.de>
#
# Information:
#  Makefile for various tools
#

prefix		= /usr/local
exec_prefix	= ${prefix}

# Compiler and Linker Options
COPTS = -O2 -Wall -g

CFLAGS	= $(COPTS) -I../getopt/ -I../ -I../lib/

GETOBJS = ../getopt/getopt.o ../getopt/getopt1.o

CC	= gcc

OBJS	= registry-assignment-to-list.o

# splint
SPLINT_OPT_OUTSIDE = -warnposix -nullassign -uniondef -compdef -usedef -formatconst -exportlocal
#SPLINT_OPT_OUTSIDE = -uniondef +matchanyintegral --nullassign +ignoresigns -compdef -usedef -modobserver -formatconst -warnposix -exportlocal
SPLINT_TODO_LATER = -unrecog  -retvalint


all:		registry-assignment-to-list

.c.o:
		$(CC) $(CFLAGS) -c $<

$(OBJS):	../config.h

libipv6calc.a:	
		cd ../ && ${MAKE} lib-make

registry-assignment-to-list:	$(OBJS) libipv6calc.a
		$(CC) -o registry-assignment-to-list $(OBJS) $(GETOBJS) -lipv6calc -L../lib/ $(LDFLAGS)

distclean:
		${MAKE} clean

autoclean:
		${MAKE} distclean

clean:
		rm -f registry-assignment-to-list *.o

test:

splint:		
		splint *.c ../lib/*.c -I.. -I../lib $(SPLINT_OPT_OUTSIDE) $(SPLINT_TODO_LATER)

install:	
