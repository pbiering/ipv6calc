#!/bin/sh
#
# Project    : ipv6calc
# File       : test_ipv6calc_filter.sh
# Version    : $Id$
# Copyright  : 2012-2015 by Peter Bieringer <pb (at) bieringer.de>
#
# Test patterns for ipv6calc filter

verbose=0
while getopts "Vh\?" opt; do
	case $opt in
	    V)
		verbose=1
		;;
	    *)
		echo "$0 [-V]"
		echo "    -V   verbose"
		exit 1
		;;
	esac
done


if [ ! -x ./ipv6calc ]; then
	echo "Binary './ipv6calc' missing or not executable"
	exit 1
fi

# Test Scenarios for autodetection "good case"
source ./test_scenarios.sh

test="run 'ipv6calc' filter tests..."
echo "INFO  : $test"
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
			echo "NOTICE: skip './ipv6calc -A filter -E $filter' for: $input (missing feature: $feature)"
			continue
		fi
	fi
		
	[ "$verbose" = "1" ] && echo "INFO  : test './ipv6calc -A filter -E $filter' for: $input"
	output="`echo "$input" | ./ipv6calc -A filter -E $filter`"
	retval=$?
	if [ $retval -ne 0 ]; then
		echo "Error executing 'ipv6calc'!"
		exit 1
	fi
	# Check result
	if [ -n "$output" ]; then
		if [ "$output" != "$input" ]; then
			echo "ERROR : result '$output' doesn't match '$input'!"
			exit 1
		else
			[ "$verbose" = "1" ] && echo "INFO  : result ok!" || true
		fi
	else
		echo "Result empty!"
		exit 1
	fi
	[ "$verbose" = "1" ] || echo -n "."
done || exit 1
[ "$verbose" = "1" ] || echo
echo "INFO  : $test successful"

# subsequent filter
echo -e "1.2.3.4\n2001:db8::1" | ./ipv6calc -E iid-local >/dev/null
if [ $? -ne 0 ]; then
	echo "ERROR : something is going wrong filtering sequence of IPv4->IPv6"
	exit 1
fi

echo -e "2001:db8::1\n1.2.3.4" | ./ipv6calc -E iid-local >/dev/null
if [ $? -ne 0 ]; then
	echo "ERROR : something is going wrong filtering sequence of IPv6->IPv4"
	exit 1
fi

test="run 'ipv6calc' test_prefix tests..."
echo "INFO  : $test"

# Pipe mode only
echo -e "1.2.3.4" | ./ipv6calc -q --test_prefix 1.2.3.0/24
if [ $? -ne 0 ]; then
	echo "ERROR : something is going wrong with IPv4 test_prefix"
	exit 1
fi

echo -e "1.2.3.4" | ./ipv6calc -q --test_prefix 1.2.4.0/24
if [ $? -ne 1 ]; then
	echo "ERROR : something is going wrong with IPv4 test_prefix"
	exit 1
fi

echo -e "2001:db8::1" | ./ipv6calc -q --test_prefix 2001:db8::/32
if [ $? -ne 0 ]; then
	echo "ERROR : something is going wrong with IPv6 test_prefix"
	exit 1
fi

echo -e "2001:db8::1" | ./ipv6calc -q --test_prefix 2001:db9::/32
if [ $? -ne 1 ]; then
	echo "ERROR : something is going wrong with IPv6 test_prefix"
	exit 1
fi

echo -e "1.2.3.4" | ./ipv6calc -q --test_prefix 2001:db8::/32
if [ $? -ne 2 ]; then
	echo "ERROR : something is going wrong with IPv4 test_prefix"
	exit 1
fi

echo -e "2001:db8::1" | ./ipv6calc -q --test_prefix 1.2.3.0/24
if [ $? -ne 2 ]; then
	echo "ERROR : something is going wrong with IPv6 test_prefix"
	exit 1
fi

echo "INFO  : $test successful"

echo "INFO  : all ipv6calc filter tests successful"

exit 0
