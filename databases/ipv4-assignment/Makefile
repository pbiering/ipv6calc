# Project    : ipv6calc/databases/ipv4-assignment
# File       : Makefile
# Version    : $Id: Makefile,v 1.5 2002/03/24 21:43:22 peter Exp $
# Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
#
# Information:
#  Makefile for ipv4-assignment

all:
		make update
		echo "Generate new header file, if necessary"
		test -f dbipv4addr_assignement.h || ./create-registry-list.pl
		find iana    -name 'ipv4-address-space' -newer dbipv4addr_assignement.h || ./create-registry-list.pl
		find arin    -name 'arin*' -newer dbipv4addr_assignement.h              || ./create-registry-list.pl
		find apnic   -name 'apnic*' -newer dbipv4addr_assignement.h             || ./create-registry-list.pl
		find ripencc -name 'ripencc*' -newer dbipv4addr_assignement.h           || ./create-registry-list.pl

install:
		echo "Nothing to do"

update:
		echo "Download new version of files"
		./update-ipv4-assignment.sh

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
