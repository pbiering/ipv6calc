# Generated automatically from Makefile.in by configure.
#
# Project    : ipv6calc/databases/ieee-oui
# File       : Makefile
# Version    : $Id: Makefile,v 1.1 2002/03/23 00:09:50 peter Exp $
# Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
#
# Information:
#  Makefile for ieee-oui

FILE = "oui.txt"
BASEURL = "http://standards.ieee.org/regauth/oui/"

all:
		echo "Nothing to do"

install:
		echo "Nothing to do"

update:
		rm -f $(FILE) $(FILE).timestamp
		wget "$(BASEURL)$(FILE)"
		date '+%s' > $(FILE).timestamp 

distclean:
		echo "Nothing to do"

autoclean:
		echo "Nothing to do"

clean:
		echo "Nothing to do"
