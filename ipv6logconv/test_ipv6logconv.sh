#!/bin/sh
#
# Project    : ipv6calc
# File       : test_ipv6logconv.sh
# Version    : $Id: test_ipv6logconv.sh,v 1.5 2005/02/12 16:33:54 peter Exp $
# Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
#
# Test program for "ipv6logconv"

testscenarios() {
# Address
cat <<END | grep -v "^#"
195.226.187.50	- - "IPv4 address"
3ffe:ffff::1	- - "IPv6 address/6bone test"
3FFE:1a05:510:200:0:5EFE:8CAD:8108 - - 
ff02::1:ff00:1234 - -
ff01::1:ff00:1234 - -
3ffe::1:ff00:1234 - -
::1 - -
::1.2.3.4 - -
::ffff:1.2.3.4 - -
2002:0102:0204::1 - -
fe80::210:a4ff:fe01:2345 - -
fe80::210:a489:ab01:2345 - -
3ffe:ffff::210:a4ff:fe01:2345 - -
3ffe:ffff::210:a489:ab01:2345 - -
3ffe:ffff::000:a4ff:fe01:2345 - -
3ffe:ffff::000:a489:ab01:2345 - -
3ffe:ffff::000:0250:c2ff:fe00:3011 - - "Microsoft"
fe80::fefc:acff:fe10:fe01 - -
1.2.3.4 - -
2002:50b5:7940:0000:0000:0000:50b5:7940 - - "Microsoft 6to4 address"
END
}

testscenarios_cache() {
# Address
cat <<END_CACHE | grep -v "^#"
195.226.187.50	- - "IPv4 address"
195.226.187.50	- - "IPv4 address"
3ffe:ffff::1	- - "IPv6 address/6bone test"
3ffe:ffff::1	- - "IPv6 address/6bone test"
3ffe:ffff::1	- - "IPv6 address/6bone test"
195.226.187.50	- - "IPv4 address"
3FFE:1a05:510:200:0:5EFE:8CAD:8108 - - 
3FFE:1a05:510:200:0:5EFE:8CAD:8108 - - 
3FFE:1a05:510:200:0:5EFE:8CAD:8108 - - 
195.226.187.50	- - "IPv4 address"
3ffe:ffff::1	- - "IPv6 address/6bone test"
3FFE:1a05:510:200:0:5EFE:8CAD:8108 - - 
1.2.3.4 - -
1.2.3.4 - -
195.226.187.50	- - "IPv4 address"
::ffff:1.2.3.4 - -
2002:0102:0204::1 - -
fe80::210:a4ff:fe01:2345 - -
fe80::210:a489:ab01:2345 - -
3ffe:ffff::210:a4ff:fe01:2345 - -
1.2.3.4 - -
2.2.3.4 - -
3.2.3.4 - -
4.2.3.4 - -
5.2.3.4 - -
195.226.187.50	- - "IPv4 address"
END_CACHE
}

#set -x
## main ##
echo "Run 'ipv6logconv' function tests..." >&2

if [ "$1" != "bulk" ]; then
	testscenarios | grep -v "^#" | while read line; do
		echo
		echo "INPUT: $line"
		echo "$line" | ./ipv6logconv --out any -q $*
		retval=$?
		if [ $retval -ne 0 ]; then
			echo "Error executing 'ipv6logconv'!" >&2
			exit 1
		fi
	done
	echo 
else
	shift
	echo "Test with cache"
	testscenarios_cache | grep -v "^#" | ./ipv6logconv --out any -q $*
	retval=$?
	if [ $retval -ne 0 ]; then
		echo "Error executing 'ipv6logconv'!" >&2
		exit 1
	fi
	echo "Test without cache"
	testscenarios_cache | grep -v "^#" | ./ipv6logconv --out any -q -n $*
	retval=$?
	if [ $retval -ne 0 ]; then
		echo "Error executing 'ipv6logconv'!" >&2
		exit 1
	fi
	echo
fi

if [ $? -eq 0 ]; then
	echo "All tests were successfully done!" >&2
fi
