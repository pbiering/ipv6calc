# Project    : ipv6calc/databases/ieee-iab
# File       : Makefile
# Version    : $Id: Makefile,v 1.1 2004/10/30 12:00:00 peter Exp $
# Copyright  : 2004 by Peter Bieringer <pb (at) bieringer.de>
#
# Information:
#  Makefile for ieee-iab

FILE = "iab.txt"
BASEURL = "http://standards.ieee.org/regauth/oui/"

all:
		test -f dbieee_iab.h || make update

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
