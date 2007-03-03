# Project    : ipv6calc/databases/ipv4-assignment
# File       : Makefile
# Version    : $Id: Makefile,v 1.18 2007/03/03 11:57:28 peter Exp $
# Copyright  : 2002-2007 by Peter Bieringer <pb (at) bieringer.de>
#
# Information:
#  Makefile for ipv4-assignment

all:
		test -f dbipv4addr_assignment.h || ${MAKE} update

install:
		echo "Nothing to do"

update:
		echo "Generate new header file, if necessary"
		./check-run-create.sh

distclean:
		echo "Nothing to do"

autoclean:
		echo "Nothing to do"

clean:
		echo "Nothing to do"
