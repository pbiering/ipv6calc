# Generated automatically from Makefile.in by configure.
#
# Project    : ipv6calc/databases/ieee-oui
# File       : Makefile
# Version    : $Id: Makefile,v 1.1 2002/03/24 17:44:35 peter Exp $
# Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
#
# Information:
#  Makefile for ieee-oui

FILE = "oui.txt"
BASEURL = "http://standards.ieee.org/regauth/oui/"

all:
		make update
		test $(FILE) -ot dbieee_oui.h || ./create_ieee_oui_headerfile.pl

install:
		echo "Nothing to do"

update:
		echo "Download new version of file"
		wget $(BASEURL)$(FILE) --timestamp

distclean:
		echo "Remove database file"
		rm -f $(FILE)

autoclean:
		echo "Nothing to do"

clean:
		echo "Nothing to do"
