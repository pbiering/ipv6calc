# Project    : ipv6calc/databases/ipv4-assignment
# File       : Makefile
# Version    : $Id: Makefile,v 1.9 2002/03/27 22:58:46 peter Exp $
# Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
#
# Information:
#  Makefile for ipv4-assignment

all:
		#make update
		echo "Generate new header file, if necessary"
		test -f dbipv4addr_assignment.h || ./create-registry-list.pl
		find iana    -type f -name 'ipv4-address-space' -newer dbipv4addr_assignment.h || ./create-registry-list.pl
		find arin    -type f -name 'arin*' -newer dbipv4addr_assignment.h              || ./create-registry-list.pl
		find apnic   -type f -name 'apnic*' -newer dbipv4addr_assignment.h             || ./create-registry-list.pl
		find ripencc -type f -name 'ripencc*' -newer dbipv4addr_assignment.h           || ./create-registry-list.pl

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
