# Project    : ipv6calc/databases/ipv6-assignment
# File       : Makefile
# Version    : $Id: Makefile,v 1.6 2015/02/15 20:19:37 ds6peter Exp $
# Copyright  : 2005-2008 by Peter Bieringer <pb (at) bieringer.de>
#               Initial copy from ../ipv4-assignment/Makefile & adaption for IPv6 by Simon Arlott
#
# Information:
#  Makefile for ipv6-assignment

all:
		test -f dbipv6addr_assignment.h || ${MAKE} update
		test -f ipv6calc-external-ipv6-registry.db || ${MAKE} update

install:
		echo "Nothing to do"

update:
		echo "Generate new header file, if necessary"
		../../tools/ipv6calc-registries-check-run-create.sh -6 -H -C ../../tools/ipv6calc-create-registry-list-ipv6.pl
		echo "Generate new db files, if necessary"
		../../tools/ipv6calc-registries-check-run-create.sh -6 -B -C ../../tools/ipv6calc-create-registry-list-ipv6.pl

updateclean:
		echo "Delete header file"
		rm -f dbipv6addr_assignment.h
		echo "Delete DB files"
		rm -f ipv6calc-external-ipv6-registry.db ipv6calc-external-ipv6-countrycode.db

distclean:
		echo "Nothing to do"

autoclean:
		echo "Nothing to do"

clean:
		echo "Nothing to do"
