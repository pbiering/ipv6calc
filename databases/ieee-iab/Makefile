# Project    : ipv6calc/databases/ieee-iab
# File       : Makefile
# Version    : $Id: Makefile,v 1.4 2008/11/19 19:57:39 peter Exp $
# Copyright  : 2004-2008 by Peter Bieringer <pb (at) bieringer.de>
#
# Information:
#  Makefile for ieee-iab

FILE = "iab.txt"
BASEURL = "http://standards.ieee.org/regauth/oui/"

all:
		test -f dbieee_iab.h || ${MAKE} update

install:
		echo "Nothing to do"

update:
		echo "Download new version of file"
		wget $(BASEURL)$(FILE) --timestamp
		./create_ieee_iab_headerfile.pl $(FILE)

updateclean:
		echo "Remove header file"
		rm -f dbieee_iab.h
		${MAKE} distclean

distclean:
		echo "Remove database file"
		rm -f $(FILE)

autoclean:
		echo "Nothing to do"

clean:
		echo "Nothing to do"
