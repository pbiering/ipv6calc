#!/bin/sh
#
# Project    : ipv6calc
# File       : test_ipv6logconv.sh
# Version    : $Id: test_ipv6logconv.sh,v 1.2 2003/11/21 09:36:58 peter Exp $
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
fe80::fefc:acff:fe10:fe01 - -
1.2.3.4 - -
END
}

#set -x
## main ##
echo "Run 'ipv6logconv' function tests..."

testscenarios | grep -v "^#" | while read line; do
	echo
	echo "INPUT: $line"
	echo "$line" | ./ipv6logconv --out any -q
	retval=$?
	if [ $retval -ne 0 ]; then
		echo "Error executing 'ipv6logconv'!"
		exit 1
	fi
done
echo 
if [ $? -eq 0 ]; then
	echo "All tests were successfully done!"
fi
