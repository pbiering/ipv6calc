# Project    : ipv6calc/databases/ipv4-assignment
# File       : Makefile
# Version    : $Id: Makefile,v 1.15 2005/09/15 17:55:43 peter Exp $
# Copyright  : 2002-2004 by Peter Bieringer <pb (at) bieringer.de>
#
# Information:
#  Makefile for ipv4-assignment

all:
		test -f dbipv4addr_assignment.h || make update

install:
		echo "Nothing to do"

update:
		echo "Download new version of files"
		./update-ipv4-assignment.sh
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
