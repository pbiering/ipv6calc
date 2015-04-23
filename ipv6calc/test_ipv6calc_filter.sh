#!/bin/sh
#
# Project    : ipv6calc
# File       : test_ipv6calc_filter.sh
# Version    : $Id: test_ipv6calc_filter.sh,v 1.4 2015/04/23 20:49:04 ds6peter Exp $
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

testscenarios_filter | while read input filter_feature; do
	if [ -z "$input" -o -z "$filter_feature" ]; then
		# end
		continue
	fi

	filter=${filter_feature/\#*/}
	feature=${filter_feature/*\#/}
	[ "$feature" = "$filter_feature" ] && feature=""

	if [ -n "$feature" ]; then
		if ! ./ipv6calc -v 2>&1 | grep -qw "$feature"; then
			echo "Skip './ipv6calc -A filter -E $filter' for: $input (missing feature: $feature)"
			continue
		fi
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

# subsequent filter
echo -e "1.2.3.4\n2001:db8::1" | ./ipv6calc -E iid-local
if [ $? -ne 0 ]; then
	echo "ERROR : something is going wrong filtering sequence of IPv4->IPv6"
	exit 1
fi

echo -e "2001:db8::1\n1.2.3.4" | ./ipv6calc -E iid-local
if [ $? -ne 0 ]; then
	echo "ERROR : something is going wrong filtering sequence of IPv6->IPv4"
	exit 1
fi
