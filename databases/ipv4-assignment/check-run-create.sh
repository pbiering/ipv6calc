#!/bin/sh
#
# Project    : ipv6calc/databases/ipv4-assignment
# File       : check-run-create.sh
# Version    : $Id: check-run-create.sh,v 1.3 2002/04/04 21:48:33 peter Exp $
# Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>

#set -x

test -f dbipv4addr_assignment.h || exit 1

IANA=`find iana    -type f -name 'ipv4-address-space' -newer dbipv4addr_assignment.h | wc -l`
ARIN=`find arin    -type f -name 'arin*' -newer dbipv4addr_assignment.h              | wc -l`
APNIC=`find apnic   -type f -name 'apnic*' -newer dbipv4addr_assignment.h            | wc -l`
RIPENCC=`find ripencc -type f -name 'ripencc*' -newer dbipv4addr_assignment.h        | wc -l`

if [ $IANA -ne 0 -o $ARIN -ne 0 -o $APNIC -ne 0 -o $RIPENCC -ne 0 ]; then
	exit 1
else
	exit 0
fi
