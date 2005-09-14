#!/bin/sh
#
# Project    : ipv6calc/databases/ipv4-assignment
# File       : check-run-create.sh
# Version    : $Id: check-run-create.sh,v 1.7 2005/09/14 18:25:24 peter Exp $
# Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>

#set -x

test -f dbipv4addr_assignment.h || exit 1

IANA=`find iana       -type f -name 'ipv4-address-space' -newer dbipv4addr_assignment.h | wc -l`
ARIN=`find arin       -type f -name 'delegated-arin*'    -newer dbipv4addr_assignment.h | wc -l`
APNIC=`find apnic     -type f -name 'delegated-apnic*'   -newer dbipv4addr_assignment.h | wc -l`
RIPENCC=`find ripencc -type f -name 'delegated-ripencc*' -newer dbipv4addr_assignment.h | wc -l`
LACNIC=`find lacnic   -type f -name 'delegated-lacnic*'  -newer dbipv4addr_assignment.h | wc -l`

echo "Found newer than dbipv4addr_assignment.h files: IANA=$IANA ARIN=$ARIN APNIC=$APNIC RIPENCC=$RIPENCC LACNIC=$LACNIC"

if [ $IANA -gt 0 -o $ARIN -gt 0 -o $APNIC -gt 0 -o $RIPENCC -gt 0 ]; then
	./create-registry-list.pl
fi
