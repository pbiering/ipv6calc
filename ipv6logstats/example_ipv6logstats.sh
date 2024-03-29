#!/bin/sh
#
# Project    : ipv6calc/logstats
# File       : example_ipv6logstats.sh
# Version    : $Id$
# Copyright  : 2003-2014 by Peter Bieringer <pb (at) bieringer.de>
#
# Example program for "ipv6logstats"


# Print header line
#./ipv6logstats -o -p "Date"

# Find logfiles (here name in format hostname-log.%Y%m(.token)(.gz|bz2)
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
	for f in $*; do
		echo "Proceed file: $f" >&2

		if echo "$f" | grep -q "\.gz$"; then
			zcat "$f"
		elif echo "$f" | grep -q "\.bz2$"; then
			bzcat "$f"
		else
			cat "$f"
		fi
	done
}

echo "Analyze Year/Month of files" >&2
find $dir -name '*-log.*' -type f | while read file; do
	#echo "Analyze Year/Month of file: $file" >&2

	# Extract %Y%m
	yearmonth="`basename "$file" | sed 's/.*\.\([0-9]\{6\}\)\..*/\1/'`"
	if [ ${#yearmonth} -eq 6 ]; then
		echo $yearmonth
	fi
done | sort -u | while read yearmonth; do
	# don't count mon and check_http (from Nagios)
	cat_file `find $dir -name "*-log.$yearmonth*" -type f` | grep -Ev "(mon.d/http.monitor|check_http)" | $bin -c -q -n -p "$yearmonth" || exit 1
done
