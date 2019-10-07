# Project    : ipv6calc/databases/ieee-oui
# File       : Makefile
# Version    : $Id$
# Copyright  : 2002-2019 by Peter Bieringer <pb (at) bieringer.de>
#
# Information:
#  Makefile for ieee-oui

FILE = oui.csv
BASEURL = http://standards-oui.ieee.org/oui/
FILE_H  = dbieee_oui.h
TYPE    = oui

all:
		test -f dbieee_oui.h || ${MAKE} update

install:
		echo "Nothing to do"

create:
		../tools/create_ieee_headerfile.pl -c -t $(TYPE) -i $(FILE) -o $(FILE_H)

update:
		echo "Download new version of file"
		wget $(BASEURL)$(FILE) --timestamp
		${MAKE} create

updateclean:
		echo "Remove header file"
		rm -f dbieee_oui.h
		${MAKE} distclean

distclean:
		echo "Remove database file"
		rm -f $(FILE)

autoclean:
		echo "Nothing to do"

clean:
		echo "Nothing to do"
