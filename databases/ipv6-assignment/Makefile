# Project    : ipv6calc/databases/ipv6-assignment
# File       : Makefile
# Version    : $Id: Makefile,v 1.1 2005/07/19 15:48:33 peter Exp $
# Copyright  : 2005 by Simon Arlott
#
# Information:
#  Makefile for ipv6-assignment
#  Modified from ipv4-assignment


all:
		test -f dbipv6addr_assignment.h || make update

install:
		echo "Nothing to do"

update:
		echo "Download new version of files"
		./update-ipv6-assignment.sh
		echo "Generate new header file, if necessary"
		./check-run-create.sh && ./create-registry-list.pl

distclean:
		echo "Delete data files"
		rm -f ipv6-unicast-address-assignments

autoclean:
		echo "Nothing to do"

clean:
		echo "Nothing to do"
