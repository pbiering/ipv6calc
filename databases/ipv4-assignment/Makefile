# Project    : ipv6calc/databases/ipv4-assignment
# File       : Makefile
# Version    : $Id: Makefile,v 1.17 2005/11/19 21:14:38 peter Exp $
# Copyright  : 2002-2005 by Peter Bieringer <pb (at) bieringer.de>
#
# Information:
#  Makefile for ipv4-assignment

all:
		test -f dbipv4addr_assignment.h || ${MAKE} update

install:
		echo "Nothing to do"

update:
		echo "Generate new header file, if necessary"
		./check-run-create.sh

distclean:
		echo "Delete data files"
		cd iana    && rm -f ipv4-address-space
		cd ripencc && rm -f delegated-ripencc-*
		cd apnic   && rm -f delegated-apnic-*
		cd arin    && rm -f delegated-arin-*

autoclean:
		echo "Nothing to do"

clean:
		echo "Nothing to do"
