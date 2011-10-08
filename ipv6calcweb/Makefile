# Project    : ipv6calc/ipv6calcweb
# File       : Makefile
# Version    : $Id: Makefile,v 1.10 2011/10/08 14:33:23 peter Exp $
# Copyright  : 2002-2011 by Peter Bieringer <pb (at) bieringer.de>
#
# Information:
#  Makefile for ipv6calcweb
#

all:		ipv6calcweb

ipv6calcweb:

update:

distclean:
		${MAKE} clean

autoclean:
		${MAKE} distclean

clean:

test:
		# Pseudo random selected
		./test_ipv6calcweb.sh 2001:200:1:ea::1
		# www.hp.com
		./test_ipv6calcweb.sh 15.200.30.23
		# Pseudo random
		./test_ipv6calcweb.sh 3ffe:831f:ce49:7601:8000:efff:af4a:86BF

test-minimal:
		${MAKE} test

