# Project    : ipv6calc/databases/ipv4-assignment
# File       : Makefile
# Version    : $Id: Makefile,v 1.11 2002/04/04 22:00:52 peter Exp $
# Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
#
# Information:
#  Makefile for ipv4-assignment

all:
		#make update

install:
		echo "Nothing to do"

update:
		echo "Download new version of files"
		./update-ipv4-assignment.sh
		echo "Generate new header file, if necessary"
		./check-run-create.sh || ./create-registry-list.pl

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
