#!/bin/sh
#
# Project    : ipv6calc/logstats
# File       : example_ipv6logstats.sh
# Version    : $Id: example_ipv6logstats.sh,v 1.1 2003/06/15 13:33:41 peter Exp $
# Copyright  : 2003 by Peter Bieringer <pb (at) bieringer.de>
#
# Example program for "ipv6logstats"


# Print header line
./ipv6logstats -o -p "Date"

# Find logfiles (here name in format hostname-log.%Y%m
find ../../*-log.* -type f | while read file; do
	echo "Proceed file: $file" >&2

	# Extract %Y%m
	yearmonth="`basename "$file" | sed 's/^.*-log\.//'`"

	cat "$file" | ./ipv6logstats -c -n -p "$yearmonth"
done
