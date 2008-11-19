# Project    : ipv6calc/databases/ipv6-assignment
# File       : Makefile
# Version    : $Id: Makefile,v 1.5 2008/11/19 19:54:47 peter Exp $
# Copyright  : 2005-2008 by Peter Bieringer <pb (at) bieringer.de>
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

updateclean:
		echo "Delete header file"
		rm -f dbipv6addr_assignment.h

distclean:
		echo "Nothing to do"

autoclean:
		echo "Nothing to do"

clean:
		echo "Nothing to do"
