#!/bin/sh
#
# Project    : ipv6calc
# File       : test_ipv6calc_filter.sh
# Version    : $Id: test_ipv6calc_filter.sh,v 1.2 2013/02/24 19:12:14 ds6peter Exp $
# Copyright  : 2012-2013 by Peter Bieringer <pb (at) bieringer.de>
#
# Test patterns for ipv6calc filter

if [ ! -x ./ipv6calc ]; then
	echo "Binary './ipv6calc' missing or not executable"
	exit 1
fi

# Test Scenarios for autodetection "good case"
source ./test_scenarios.sh

echo "Run 'ipv6calc' filter tests..."

testscenarios_filter | tr A-Z a-z | while read input filter; do
	if [ -z "$input" -o -z "$filter" ]; then
		# end
		continue
	fi

	echo "Test './ipv6calc -A filter -E $filter' for: $input"
	output="`echo "$input" | ./ipv6calc -A filter -E $filter`"
	retval=$?
	if [ $retval -ne 0 ]; then
		echo "Error executing 'ipv6calc'!"
		exit 1
	fi
	# Check result
	if [ -n "$output" ]; then
		if [ "$output" != "$input" ]; then
			echo "Result '$output' doesn't match '$input'!"
			exit 1
		else
			echo "Result ok!"
		fi
	else
		echo "Result empty!"
		exit 1
	fi
done || exit 1 
