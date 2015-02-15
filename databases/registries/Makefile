# Project    : ipv6calc/databases/registries
# File       : Makefile
# Version    : $Id: Makefile,v 1.8 2015/02/15 20:19:37 ds6peter Exp $
# Copyright  : 2002-2015 by Peter Bieringer <pb (at) bieringer.de>
#               replaces parts of ../ipv4-assignment/Makefile
#               replaces parts of ../ipv6-assignment/Makefile
#
# Information:
#  Makefile for updating assignment data of registries

all:
		${MAKE} update

install:
		echo "Nothing to do"

download:
		echo "Download new version of files"
		../../tools/ipv6calc-update-registries.sh

update-only:
		echo "Create database files for IPv4"
		cd ../ipv4-assignment && ${MAKE} update

		echo "Create database files for IPv6"
		cd ../ipv6-assignment && ${MAKE} update

		echo "Create database files for ASN to Registry"
		cd ../as-assignment && ${MAKE} update

		echo "Create database files for CountryCode to Registry"
		cd ../cc-assignment && ${MAKE} update

update:
		${MAKE} download

		${MAKE} update-only

distclean:
		echo "Delete data files"
		cd iana    && rm -f ipv6-unicast-address-assignments.xml ipv4-address-space.xml as-numbers.txt
		cd ripencc && rm -f delegated-ripencc-*
		cd apnic   && rm -f delegated-apnic-*
		cd arin    && rm -f delegated-arin-*
		cd lacnic  && rm -f delegated-lacnic-*
		cd afrinic && rm -f delegated-afrinic-*

autoclean:
		echo "Nothing to do"

clean:
		echo "Nothing to do"
