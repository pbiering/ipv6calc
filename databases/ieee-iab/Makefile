# Project    : ipv6calc/databases/ieee-iab
# File       : Makefile
# Version    : $Id$
# Copyright  : 2004-2019 by Peter Bieringer <pb (at) bieringer.de>
#
# Information:
#  Makefile for ieee-iab

FILE    = iab.txt
BASEURL = http://standards-oui.ieee.org/iab/
FILE_H  = dbieee_iab.h
TYPE    = iab

all:
		test -f $(FILE_H) || ${MAKE} update

install:
		echo "Nothing to do"

create:
		../tools/create_ieee_headerfile.pl -t $(TYPE) -i $(FILE) -o $(FILE_H)

update:
		echo "Download new version of file"
		wget $(BASEURL)$(FILE) --timestamp
		${MAKE} create

updateclean:
		echo "Remove header file"
		rm -f $(FILE_H)
		${MAKE} distclean

distclean:
		echo "Remove database file"
		rm -f $(FILE)

autoclean:
		echo "Nothing to do"

clean:
		echo "Nothing to do"
