# Project    : ipv6calc/databases/ipv4-assignment
# File       : Makefile
# Version    : $Id$
# Copyright  : 2005-2015 by Peter Bieringer <pb (at) bieringer.de>
#
# Information:
#  Makefile for ipv4-assignment

all:
		test -f dbipv4addr_assignment.h || ${MAKE} update

install:
		echo "Nothing to do"

update:
		echo "Generate new header file, if necessary"
		../../tools/ipv6calc-registries-check-run-create.sh -4 -H -C ../../tools/ipv6calc-create-registry-list-ipv4.pl

force-update:
		echo "Generate new header file"
		../../tools/ipv6calc-registries-check-run-create.sh -4 -H -C ../../tools/ipv6calc-create-registry-list-ipv4.pl

db:
		echo "Generate new db files, if necessary"
		../../tools/ipv6calc-registries-check-run-create.sh -4 -B -C ../../tools/ipv6calc-create-registry-list-ipv4.pl

force-db:
		echo "Generate new db files"
		../../tools/ipv6calc-registries-check-run-create.sh -u -4 -B -C ../../tools/ipv6calc-create-registry-list-ipv4.pl

updateclean:
		echo "Delete header file"
		rm -f dbipv4addr_assignment.h
		echo "Delete DB files"
		rm -f ipv6calc-external-ipv4-registry.db ipv6calc-external-ipv4-countrycode.db

distclean:
		echo "Nothing to do"

autoclean:
		echo "Nothing to do"

clean:
		echo "Nothing to do"
