# Project    : ipv6calc/databases/ieee-iab
# File       : Makefile
# Version    : $Id: Makefile,v 1.2 2005/11/19 21:14:38 peter Exp $
# Copyright  : 2004-2005 by Peter Bieringer <pb (at) bieringer.de>
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

distclean:
		echo "Remove database file"
		rm -f $(FILE)

autoclean:
		echo "Nothing to do"

clean:
		echo "Nothing to do"
