# Project    : ipv6calc
# File       : lib/Makefile
# Version    : $Id: Makefile,v 1.8 2013/08/11 16:42:11 ds6peter Exp $
# Copyright  : 2013-2013 by Peter Bieringer <pb (at) bieringer.de>
#
# Information:
#  Makefile for ipv6calc database libraries
#

# Compiler and Linker Options
#  -D__EXTENSIONS__ is for Solaris & strtok_r
DEFAULT_CFLAGS ?= -O2 -Wall
CFLAGS += -D__EXTENSIONS__

INCLUDES= -I../../lib -I../.. $(GETOPT_INCLUDE)

CC	= gcc
AR	= ar
RANLIB	= ranlib

OBJS	=	libipv6calc_db_wrapper.o \
		libipv6calc_db_wrapper_GeoIP.o \
		libipv6calc_db_wrapper_BuiltIn.o

all:		libipv6calc_db_wrapper.a

.c.o:
		$(CC) $(DEFAULT_CFLAGS) $(CFLAGS) $(INCLUDES) -c $<

$(OBJS):	libipv6calc_db_wrapper.h \
		libipv6calc_db_wrapper_GeoIP.h \
		libipv6calc_db_wrapper_BuiltIn.h

libipv6calc_db_wrapper.a:	$(OBJS)
		echo "Create library"
		$(AR) r libipv6calc_db_wrapper.a $(OBJS)
		$(RANLIB) libipv6calc_db_wrapper.a

distclean:
		${MAKE} clean

autoclean:
		${MAKE} distclean

clean:
		rm -f *.o *.a

install:

test:
