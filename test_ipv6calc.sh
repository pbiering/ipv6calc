#!/bin/sh
#
# Testfile for ipv6calc
#
# Version:     $Id: test_ipv6calc.sh,v 1.1.1.1 2001/10/07 14:47:40 peter Exp $
#
# Author:      Peter Bieringer <pb@bieringer.de>
#
# Best view with tabsize 4
#


testscenarios() {
# Command													Expected result (no space between "=" and result)
cat <<END
--addr_to_ip6int 3ffe:400:100:f101::1								=1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.1.0.1.f.0.0.1.0.0.0.4.0.e.f.f.3.ip6.int
--addr_to_ip6int 3ffe:400:100:f101::1/64							=1.0.1.f.0.0.1.0.0.0.4.0.e.f.f.3.ip6.int
--addr_to_compressed 3ffe:0400:0100:f101:0000:0000:0000:0001		=3ffe:400:100:f101::1
--addr_to_compressed 3ffe:0400:0100:f101:0000:0000:0000:0001/64		=3ffe:400:100:f101::1/64
--addr_to_compressed	0:0:1:2:3:4:0:0								=::1:2:3:4:0:0
--addr_to_compressed 0:0:1:0:0:1:0:0								=::1:0:0:1:0:0
--addr_to_compressed 0:0:0:2:3:0:0:0								=::2:3:0:0:0
--addr_to_compressed 0:0:0:0:3:0:0:0								=::3:0:0:0
--addr_to_compressed 0:0:0:0:0:4:0:0								=::4:0:0
--addr_to_compressed 0:0:1:0:0:1:0:0								=::1:0:0:1:0:0
--addr_to_compressed 0:0:1:2:0:0:0:0								=0:0:1:2::
--addr_to_compressed 0:0:1:2:3:0:0:0								=0:0:1:2:3::
--addr_to_compressed 1080:0:0:0:8:800:200c:417a						=1080::8:800:200c:417a
--addr_to_compressed ff01:0:0:0:0:0:0:43							=ff01::43
--addr_to_compressed 0:0:0:0:0:0:0:1								=::1
--addr_to_compressed 0:0:0:0:0:0:0:0								=::
--addr_to_compressed 0:0:0:0:0:0:13.1.68.3							=::13.1.68.3
--addr_to_compressed 0:0:0:0:0:ffff:129.144.52.38					=::ffff:129.144.52.38
--addr_to_uncompressed 3ffe:400:100:f101::1							=3ffe:400:100:f101:0:0:0:1
--addr_to_uncompressed 3ffe:400:100:f101::1/64						=3ffe:400:100:f101:0:0:0:1/64
--addr_to_uncompressed ::13.1.68.3									=0:0:0:0:0:0:13.1.68.3
--addr_to_uncompressed ::013.01.068.0003							=0:0:0:0:0:0:13.1.68.3
--addr_to_uncompressed ::ffff:13.1.68.3								=0:0:0:0:0:ffff:13.1.68.3
--addr_to_uncompressed --maskprefix 3ffe:400:100:f101::1/64			=3ffe:400:100:f101:0:0:0:0/64
--addr_to_uncompressed --masksuffix 3ffe:400:100:f101:c000::1/64	=0:0:0:0:c000:0:0:1/64
--addr_to_fulluncompressed 3ffe:400:100:f101::1						=3ffe:0400:0100:f101:0000:0000:0000:0001
--addr_to_fulluncompressed 3ffe:400:100:f101::1/64					=3ffe:0400:0100:f101:0000:0000:0000:0001/64
--addr_to_fulluncompressed ::13.1.68.3								=0000:0000:0000:0000:0000:0000:13.1.68.3
--addr_to_fulluncompressed ::ffff:13.1.68.3							=0000:0000:0000:0000:0000:ffff:13.1.68.3
--addr_to_ifinet6 3ffe:400:100:f101::1								=3ffe04000100f1010000000000000001 00
--addr_to_ifinet6 3ffe:400:100:f101::1/64							=3ffe04000100f1010000000000000001 00 40
--ifinet6_to_compressed 3ffe04000100f1010000000000000001			=3ffe:400:100:f101::1
--ifinet6_to_compressed 3ffe04000100f1010000000000000001 40			=3ffe:400:100:f101::1/64
--addr_to_base85 1080:0:0:0:8:800:200c:417a							=4)+k&C#VzJ4br>0wv%Yp
--base85_to_addr 4)+k&C#VzJ4br>0wv%Yp								=1080:0:0:0:8:800:200c:417a
--mac_to_eui64 00:50:BF:06:B4:F5									=250:bfff:fe06:b4f5
--mac_to_eui64 00:0:F:6:4:5											=200:fff:fe06:405
--addr_to_uncompressed --printsuffix ::ffff:1.2.3.4/64				=0:ffff:1.2.3.4
--addr_to_uncompressed --printsuffix ::ffff:1.2.3.4/63				=0:0:ffff:1.2.3.4
--addr_to_uncompressed --printsuffix 3ffe:400:100:f101::1/64		=0:0:0:1
--addr_to_uncompressed --printprefix 3ffe:400:100:f101::1/64		=3ffe:400:100:f101
--addr_to_uncompressed --printprefix 3ffe:400:100:f101::1/65		=3ffe:400:100:f101:0
--eui64_to_privacy 0123456789abcdef 0123456789abcdef				=4462bdea8654776d 486072ff7074945e
END
}

#set -x
## main ##
echo "Run 'ipv6calc' function tests..."

testscenarios | while read line; do
	# extract result
	command="`echo $line | awk -F= '{ print $1 }'`"
	result="`echo $line | awk -F= '{ print $2 }'`"
	if [ -z "$result" -o -z "$command" ]; then
		echo "Something is wrong in line '$line'"
		continue
	fi
	echo "Test './ipv6calc $command' for '$result'"
	# get result
	output="`./ipv6calc $command`"
	retval=$?
	if [ $retval -ne 0 ]; then
		echo "Error executing 'ipv6calc'!"
		exit 1
	fi
	# Check result
	if [ "$output" != "$result" ]; then
		echo "Result '$output' doesn't match!"
		exit 1
	fi	
done

if [ $? -eq 0 ]; then
	echo "All tests were successfully done!"
fi
