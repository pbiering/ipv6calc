# Project    : ipv6calc/databases/ipv4-assignment
# File       : Makefile
# Version    : $Id: Makefile,v 1.21 2015/02/15 20:19:37 ds6peter Exp $
# Copyright  : 2005-2015 by Peter Bieringer <pb (at) bieringer.de>
#
# Information:
#  Makefile for ipv4-assignment

all:
		test -f dbipv4addr_assignment.h || ${MAKE} update
		test -f ipv6calc-external-ipv4-registry.db || ${MAKE} update

install:
		echo "Nothing to do"

force-update:
		${MAKE} updateclean
		${MAKE} update

update:
		echo "Generate new header file, if necessary"
		../../tools/ipv6calc-registries-check-run-create.sh -4 -H -C ../../tools/ipv6calc-create-registry-list-ipv4.pl
		echo "Generate new db files, if necessary"
		../../tools/ipv6calc-registries-check-run-create.sh -4 -B -C ../../tools/ipv6calc-create-registry-list-ipv4.pl

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
