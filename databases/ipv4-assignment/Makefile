# Project    : ipv6calc/databases/ipv4-assignment
# File       : Makefile
# Version    : $Id: Makefile,v 1.1 2002/03/24 16:54:56 peter Exp $
# Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
#
# Information:
#  Makefile for ipv4-assignment

all:
		make update
		echo "Generate new header file, if necessary"
		test dbipv4addr_assignement.h -ot iana/    || ./create-registry-list.pl
		test dbipv4addr_assignement.h -ot ripencc/ || ./create-registry-list.pl
		test dbipv4addr_assignement.h -ot arin/    || ./create-registry-list.pl
		test dbipv4addr_assignement.h -ot apnic/   || ./create-registry-list.pl

install:
		echo "Nothing to do"

update:
		echo "Download new version of files"
		./update-ipv4-assignment.sh

distclean:
		echo "Delete data files"
		cd iana    && rm -f ipv4-address-spac
		cd ripencc && rm -f ripencc.*
		cd apnic   && rm -f apnic-*
		cd arin    && rm -f arin.*

autoclean:
		echo "Nothing to do"

clean:
		echo "Nothing to do"
