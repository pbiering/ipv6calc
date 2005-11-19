# Project    : ipv6calc/databases/ipv6-assignment
# File       : Makefile
# Version    : $Id: Makefile,v 1.3 2005/11/19 21:14:38 peter Exp $
# Copyright  : 2005 by Peter Bieringer <pb (at) bieringer.de>
#               Initial copy from ../ipv4-assignment/Makefile & adaption for IPv6 by Simon Arlott
#
# Information:
#  Makefile for ipv6-assignment

all:
		test -f dbipv6addr_assignment.h || ${MAKE} update

install:
		echo "Nothing to do"

update:
		echo "Generate new header file, if necessary"
		./check-run-create.sh

distclean:
		echo "Delete data files"
		rm -f ipv6-unicast-address-assignments

autoclean:
		echo "Nothing to do"

clean:
		echo "Nothing to do"
