# Project    : ipv6calc/databases/ipv4-assignment
# File       : Makefile
# Version    : $Id: Makefile,v 1.14 2005/07/14 08:21:41 peter Exp $
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
		cd ripencc && rm -f ripencc.*
		cd apnic   && rm -f apnic-*
		cd arin    && rm -f arin.*

autoclean:
		echo "Nothing to do"

clean:
		echo "Nothing to do"
