# Project    : ipv6calc/databases/registries
# File       : Makefile
# Version    : $Id: Makefile,v 1.1 2005/10/23 21:22:54 peter Exp $
# Copyright  : 2002-2005 by Peter Bieringer <pb (at) bieringer.de>
#               replaces parts of ../ipv4-assignment/Makefile
#               replaces parts of ../ipv6-assignment/Makefile
#
# Information:
#  Makefile for updating assignment data of registries

all:
		make update

install:
		echo "Nothing to do"

update:
		echo "Download new version of files"
		./update-registries.sh
		echo "Create database files for IPv4"
		cd ../ipv4-assignment && make update
		echo "Create database files for IPv6"
		cd ../ipv6-assignment && make update

distclean:
		echo "Delete data files"
		cd iana    && rm -f ipv4-address-space ipv6-unicast-address-assignments
		cd ripencc && rm -f delegated-ripencc-*
		cd apnic   && rm -f delegated-apnic-*
		cd arin    && rm -f delegated-arin-*

autoclean:
		echo "Nothing to do"

clean:
		echo "Nothing to do"
