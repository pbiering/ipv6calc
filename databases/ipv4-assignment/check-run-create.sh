#!/bin/sh
#
# Project    : ipv6calc/databases/ipv4-assignment
# File       : check-run-create.sh
# Version    : $Id: check-run-create.sh,v 1.10 2005/11/19 21:44:59 peter Exp $
# Copyright  : 2002-2005 by Peter Bieringer <pb (at) bieringer.de>

#set -x

flag_update=0

if [ -f dbipv4addr_assignment.h ]; then
	IANA=`find    ../registries/iana    -type f -name 'ipv4-address-space' -newer dbipv4addr_assignment.h | wc -l`
	ARIN=`find    ../registries/arin    -type f -name 'delegated-arin*'    -newer dbipv4addr_assignment.h | wc -l`
	APNIC=`find   ../registries/apnic   -type f -name 'delegated-apnic*'   -newer dbipv4addr_assignment.h | wc -l`
	RIPENCC=`find ../registries/ripencc -type f -name 'delegated-ripencc*' -newer dbipv4addr_assignment.h | wc -l`
	LACNIC=`find  ../registries/lacnic  -type f -name 'delegated-lacnic*'  -newer dbipv4addr_assignment.h | wc -l`
	AFRINIC=`find ../registries/afrinic -type f -name 'delegated-afrinic*' -newer dbipv4addr_assignment.h | wc -l`

	echo "Found newer than dbipv4addr_assignment.h files: IANA=$IANA ARIN=$ARIN APNIC=$APNIC RIPENCC=$RIPENCC LACNIC=$LACNIC AFRINIC=$AFRINIC"

	if [ $IANA -gt 0 -o $ARIN -gt 0 -o $APNIC -gt 0 -o $RIPENCC -gt 0 -o $LACNIC -gt 0 -o $AFRINIC -gt 0 ]; then
		flag_update=1
	fi
else
	flag_update=1
fi

if [ $flag_update -eq 1 ]; then
	./create-registry-list.pl
else
	echo " Nothing to do!"
fi
