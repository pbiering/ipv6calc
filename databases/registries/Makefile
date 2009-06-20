# Project    : ipv6calc/databases/registries
# File       : Makefile
# Version    : $Id: Makefile,v 1.5 2009/06/20 13:37:59 peter Exp $
# Copyright  : 2002-2009 by Peter Bieringer <pb (at) bieringer.de>
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
		./update-registries.sh

update:
		${MAKE} download
		echo "Create database files for IPv4"
		cd ../ipv4-assignment && ${MAKE} update
		echo "Create database files for IPv6"
		cd ../ipv6-assignment && ${MAKE} update

distclean:
		echo "Delete data files"
		cd iana    && rm -f ipv4-address-space ipv6-unicast-address-assignments
		cd ripencc && rm -f delegated-ripencc-*
		cd apnic   && rm -f delegated-apnic-*
		cd arin    && rm -f delegated-arin-*
		cd lacnic  && rm -f delegated-lacnic-*
		cd afrinic && rm -f delegated-afrinic-*

autoclean:
		echo "Nothing to do"

clean:
		echo "Nothing to do"
