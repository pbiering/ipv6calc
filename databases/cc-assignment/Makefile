# Project    : ipv6calc/databases/cc-assignment
# File       : Makefile
# Version    : $Id: Makefile,v 1.1 2013/10/13 16:19:34 ds6peter Exp $
# Copyright  : 2013-2013 by Peter Bieringer <pb (at) bieringer.de>
#
# Information:
#  Makefile for cc-assignment

all:
		test -f db_cc_reg_assignment.h || ${MAKE} update

install:
		echo "Nothing to do"

update:
		echo "Generate new header file, if necessary"
		./check-run-create.sh

updateclean:
		echo "Delete header file"
		rm -f db_cc_reg_assignment.h

distclean:
		echo "Nothing to do"

autoclean:
		echo "Nothing to do"

clean:
		echo "Nothing to do"

updateforce:
		${MAKE} updateclean
		${MAKE} update
