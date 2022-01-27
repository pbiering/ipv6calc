# Project    : ipv6calc/databases/registries
# File       : Makefile
# Version    : $Id$
# Copyright  : 2002-2022 by Peter Bieringer <pb (at) bieringer.de>
#               replaces parts of ../ipv4-assignment/Makefile
#               replaces parts of ../ipv6-assignment/Makefile
#
# Information:
#  Makefile for updating assignment data of registries

all:
		${MAKE} update

install:
		echo "Nothing to do"

download:
		echo "Download new version of files"
		../../tools/ipv6calc-update-registries.sh

update-only:
		echo "Create database files for IPv4"
		cd ../ipv4-assignment && ${MAKE} update

		echo "Create database files for IPv6"
		cd ../ipv6-assignment && ${MAKE} update

		echo "Create database files for ASN to Registry"
		cd ../as-assignment && ${MAKE} update

		echo "Create database files for CountryCode to Registry"
		cd ../cc-assignment && ${MAKE} update

update:
		${MAKE} download

		${MAKE} update-only

distclean:
		echo "Delete data files"
		if [ -d iana ]; then \
			echo "Delete datafiles: iana"; \
			rm -f iana/ipv6-unicast-address-assignments.xml iana/ipv4-address-space.xml iana/as-numbers.txt; \
			rmdir iana; \
		fi
		if [ -d lisp ]; then \
			echo "Delete datafiles: lisp"; \
			rm -f lisp/site-db; \
			rmdir lisp; \
		fi
		for d in ripencc apnic arin lacnic afrinic; do \
			if [ -d $${d} ]; then \
				echo "Delete datafiles: $${d}"; \
				rm -f $${d}/delegated-$${d}-*; \
				rmdir $${d}; \
			fi; \
		done

autoclean:
		echo "Nothing to do"

clean:
		echo "Nothing to do"
