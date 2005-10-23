#!/bin/sh
#
# Project    : ipv6calc/databases/ipv6-assignment
# File       : check-run-create.sh
# Version    : $Id: check-run-create.sh,v 1.2 2005/10/23 21:22:53 peter Exp $
# Copyright  : 2005 by Peter Bieringer <pb (at) bieringer.de>

flag_update=0

if [ -f dbipv6addr_assignment.h ]; then
	flag_update=1
else
	flag_update=1
fi

if [ $flag_update -eq 1 ]; then
	./create-registry-list.pl
fi
