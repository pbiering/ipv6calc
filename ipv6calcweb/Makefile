# Project    : ipv6calc/ipv6calcweb
# File       : Makefile
# Version    : $Id: Makefile,v 1.15 2013/11/10 18:20:53 ds6peter Exp $
# Copyright  : 2002-2013 by Peter Bieringer <pb (at) bieringer.de>
#
# Information:
#  Makefile for ipv6calcweb
#

all:		ipv6calcweb

ipv6calcweb:	ipv6calcweb.cgi.in
		./create_ipv6calcweb-cgi.sh

update:

distclean:
		${MAKE} clean

autoclean:
		${MAKE} distclean

clean:
		rm -f ipv6calcweb.cgi

test:
		${MAKE} ipv6calcweb
		pushd ../ipv6calc; ${MAKE}; popd
		# Pseudo random selected
		./test_ipv6calcweb.sh 2001:200:1:ea::1 || exit 1
		# www.hp.com
		./test_ipv6calcweb.sh 15.200.30.23 || exit 1
		# Pseudo random
		./test_ipv6calcweb.sh 3ffe:831f:ce49:7601:8000:efff:af4a:86BF || exit 1
		# RFC
		./test_ipv6calcweb.sh :: || exit 1

		# Form mode
		./test_ipv6calcweb_form.sh || exit 1

test-minimal:
		${MAKE} test

