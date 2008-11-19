# Project    : ipv6calc/databases/ieee-oui
# File       : Makefile
# Version    : $Id: Makefile,v 1.10 2008/11/19 19:26:12 peter Exp $
# Copyright  : 2002-2008 by Peter Bieringer <pb (at) bieringer.de>
#
# Information:
#  Makefile for ieee-oui

FILE = oui_public.txt
BASEURL = http://standards.ieee.org/regauth/oui/

all:
		test -f dbieee_oui.h || ${MAKE} update

install:
		echo "Nothing to do"

create:
		./create_ieee_oui_headerfile.pl $(FILE)

update:
		echo "Download new version of file"
		wget $(BASEURL)$(FILE) --timestamp
		${MAKE} create

distclean:
		echo "Remove database file"
		rm -f $(FILE)

autoclean:
		echo "Nothing to do"

clean:
		echo "Nothing to do"
