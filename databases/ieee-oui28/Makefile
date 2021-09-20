# Project    : ipv6calc/databases/ieee-oui36
# File       : Makefile
# Version    : $Id$
# Copyright  : 2019-2019 by Peter Bieringer <pb (at) bieringer.de>
#
# Information:
#  Makefile for ieee-oui28

FILE    = mam.csv
BASEURL = http://standards.ieee.org/develop/regauth/oui28/
FILE_H  = dbieee_oui28.h
TYPE    = oui28

all:
		test -f $(FILE_H) || ${MAKE} update

install:
		echo "Nothing to do"

create:
		../tools/create_ieee_headerfile.pl -c -t $(TYPE) -i $(FILE) -o $(FILE_H) -l ../../lib/libieee.c

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
