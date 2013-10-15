#!/bin/sh
#
# Project    : ipv6calc/databases/cc-assignment
# File       : check-run-create.sh
# Version    : $Id: check-run-create.sh,v 1.1 2013/10/15 06:19:31 ds6peter Exp $
# Copyright  : 2013-2013 by Peter Bieringer <pb (at) bieringer.de>

#set -x

flag_update=0
file_header="dbasn_assignment.h"

if [ -f $file_header ]; then
	IANA=`find    ../registries/iana    -type f -name 'ipv4-address-space' -newer $file_header | wc -l`
	ARIN=`find    ../registries/arin    -type f -name 'delegated-arin*'    -newer $file_header | wc -l`
	APNIC=`find   ../registries/apnic   -type f -name 'delegated-apnic*'   -newer $file_header | wc -l`
	RIPENCC=`find ../registries/ripencc -type f -name 'delegated-ripencc*' -newer $file_header | wc -l`
	LACNIC=`find  ../registries/lacnic  -type f -name 'delegated-lacnic*'  -newer $file_header | wc -l`
	AFRINIC=`find ../registries/afrinic -type f -name 'delegated-afrinic*' -newer $file_header | wc -l`

	echo "Found newer than $file_header file: IANA=$IANA ARIN=$ARIN APNIC=$APNIC RIPENCC=$RIPENCC LACNIC=$LACNIC AFRINIC=$AFRINIC"

	if [ $IANA -gt 0 -o $ARIN -gt 0 -o $APNIC -gt 0 -o $RIPENCC -gt 0 -o $LACNIC -gt 0 -o $AFRINIC -gt 0 ]; then
		flag_update=1
	fi
else
	flag_update=1
fi

if [ $flag_update -eq 1 ]; then
	./create-asn-registry-list.pl
else
	echo " Nothing to do!"
fi
