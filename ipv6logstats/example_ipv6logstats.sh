#!/bin/sh
#
# Project    : ipv6calc/logstats
# File       : example_ipv6logstats.sh
# Version    : $Id: example_ipv6logstats.sh,v 1.5 2003/11/22 12:42:24 peter Exp $
# Copyright  : 2003 by Peter Bieringer <pb (at) bieringer.de>
#
# Example program for "ipv6logstats"


# Print header line
#./ipv6logstats -o -p "Date"

# Find logfiles (here name in format hostname-log.%Y%m
dir="$1"

bin="./ipv6logstats"

if [ -z "$dir" ]; then
	dir="."
fi

if [ -n "$2" ]; then
	bin="$2"
	if [ ! -x "$bin" ]; then
		echo "Error: cannot execute: $bin"
		exit 1
	fi
fi

cat_file() {
	local f="$1"

	if [ -z "$f" ]; then
		return 1
	fi

	if echo "$f" | grep -q "\.gz$"; then
		zcat "$f"
	elif echo "$f" | grep -q "\.bz2$"; then
		bzcat "$f"
	else
		cat "$f"
	fi
}

find $dir -name '*-log.*' -type f | while read file; do
	echo "Proceed file: $file" >&2

	# Extract %Y%m
	yearmonth="`basename "$file" | sed 's/^.*-log\.//' | sed 's/\.gz$//' | sed 's/\.bz2$//'`"

	# don't count mon checks
	cat_file "$file" | grep -v "mon.d/http.monitor" | $bin -c -n -p "$yearmonth"
done
