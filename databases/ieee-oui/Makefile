# Generated automatically from Makefile.in by configure.
#
# Project    : ipv6calc/databases/ieee-oui
# File       : Makefile
# Version    : $Id: Makefile,v 1.5 2002/07/17 18:25:36 peter Exp $
# Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
#
# Information:
#  Makefile for ieee-oui

#FILE = "oui.txt"
FILE = "oui_public.txt"
BASEURL = "http://standards.ieee.org/regauth/oui/"

all:
		test -f dbieee_oui.h || make update

install:
		echo "Nothing to do"

update:
		echo "Download new version of file"
		wget $(BASEURL)$(FILE) --timestamp --output-document=oui.txt
		./create_ieee_oui_headerfile.pl

distclean:
		echo "Remove database file"
		rm -f $(FILE)

autoclean:
		echo "Nothing to do"

clean:
		echo "Nothing to do"
