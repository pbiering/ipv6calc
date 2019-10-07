# Project    : ipv6calc/databases/ieee-oui36
# File       : Makefile
# Version    : $Id$
# Copyright  : 2013-2019 by Peter Bieringer <pb (at) bieringer.de>
#
# Information:
#  Makefile for ieee-oui36

FILE    = oui36.csv
BASEURL = http://standards.ieee.org/develop/regauth/oui36/
FILE_H  = dbieee_oui36.h
TYPE    = oui36

all:
		test -f $(FILE_H) || ${MAKE} update

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
		rm -f $(FILE_H)
		${MAKE} distclean

distclean:
		echo "Remove database file"
		rm -f $(FILE)

autoclean:
		echo "Nothing to do"

clean:
		echo "Nothing to do"
