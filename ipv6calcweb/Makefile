# Project    : ipv6calc/ipv6calcweb
# File       : Makefile
# Version    : $Id: Makefile,v 1.3 2002/04/05 21:12:40 peter Exp $
# Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
#
# Information:
#  Makefile for ipv6calcweb
#

all:		ipv6calcweb

ipv6calcweb:
		test -x /usr/bin/perl && make update

update:
		echo "Update version: ipv6calcweb.cgi"
		make clean
		./create_version.pl >ipv6calcweb.cgi.version
		mv ipv6calcweb.cgi.version ipv6calcweb.cgi
		chmod 750 ipv6calcweb.cgi

distclean:
		make clean

clean:
		rm -f ipv6calcweb.cgi.version

test:
		echo "Currently missing"
