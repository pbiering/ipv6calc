#!/bin/sh
#
# Project    : ipv6calc
# File       : test_ipv6calc.sh
# Version    : $Id: test_ipv6calc.sh,v 1.6 2002/03/01 23:26:45 peter Exp $
# Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
#
# Best view with tabsize 4 (historic...)
#


testscenarios() {
# Command													Expected result (no space between "=" and result)
cat <<END
--addr_to_ip6int 3ffe:ffff:100:f101::1					=1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.1.0.1.f.0.0.1.0.f.f.f.f.e.f.f.3.ip6.int.
--intype ipv6 --outtype revnibbles.int 3ffe:ffff:100:f101::1		=1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.1.0.1.f.0.0.1.0.f.f.f.f.e.f.f.3.ip6.int.
--addr_to_ip6int 3ffe:ffff:100:f101::1/64				=1.0.1.f.0.0.1.0.f.f.f.f.e.f.f.3.ip6.int.
--addr_to_compressed 3ffe:ffff:0100:f101:0000:0000:0000:0001		=3ffe:ffff:100:f101::1
--addr_to_compressed 3ffe:ffff:0100:f101:0000:0000:0000:0001/64		=3ffe:ffff:100:f101::1/64
--addr_to_compressed 0:0:1:2:3:4:0:0					=::1:2:3:4:0:0
--addr_to_compressed 0:0:1:0:0:1:0:0					=::1:0:0:1:0:0
--addr_to_compressed 0:0:0:2:3:0:0:0					=::2:3:0:0:0
--addr_to_compressed 0:0:0:0:3:0:0:0					=::3:0:0:0
--addr_to_compressed 0:0:0:0:0:4:0:0					=::4:0:0
--addr_to_compressed 0:0:1:0:0:1:0:0					=::1:0:0:1:0:0
--addr_to_compressed 0:0:1:2:0:0:0:0					=0:0:1:2::
--addr_to_compressed 0:0:1:2:3:0:0:0					=0:0:1:2:3::
--addr_to_compressed 1080:0:0:0:8:800:200c:417a				=1080::8:800:200c:417a
--addr_to_compressed ff01:0:0:0:0:0:0:43				=ff01::43
--addr_to_compressed 0:0:0:0:0:0:0:1					=::1
--addr_to_compressed 0:0:0:0:0:0:0:0					=::
--addr_to_compressed 0:0:0:0:0:0:13.1.68.3				=::13.1.68.3
--addr_to_compressed 0:0:0:0:0:ffff:129.144.52.38			=::ffff:129.144.52.38
--addr_to_compressed --uppercase 0:0:0:0:0:ffff:129.144.52.38		=::FFFF:129.144.52.38
--addr_to_uncompressed 3ffe:ffff:100:f101::1				=3ffe:ffff:100:f101:0:0:0:1
--addr_to_uncompressed 3ffe:ffff:100:f101::1/64				=3ffe:ffff:100:f101:0:0:0:1/64
--addr_to_uncompressed ::13.1.68.3					=0:0:0:0:0:0:13.1.68.3
--addr_to_uncompressed ::013.01.068.0003				=0:0:0:0:0:0:13.1.68.3
--addr_to_uncompressed ::ffff:13.1.68.3					=0:0:0:0:0:ffff:13.1.68.3
--addr_to_uncompressed --maskprefix 3ffe:ffff:100:f101::1/64		=3ffe:ffff:100:f101:0:0:0:0/64
--addr_to_uncompressed --masksuffix 3ffe:ffff:100:f101:c000::1/64	=0:0:0:0:c000:0:0:1/64
--addr_to_uncompressed --uppercase ::ffff:13.1.68.3			=0:0:0:0:0:FFFF:13.1.68.3
--addr_to_fulluncompressed 3ffe:ffff:100:f101::1			=3ffe:ffff:0100:f101:0000:0000:0000:0001
--addr_to_fulluncompressed 3ffe:ffff:100:f101::1/64			=3ffe:ffff:0100:f101:0000:0000:0000:0001/64
--addr_to_fulluncompressed ::13.1.68.3					=0000:0000:0000:0000:0000:0000:13.1.68.3
--addr_to_fulluncompressed ::ffff:13.1.68.3				=0000:0000:0000:0000:0000:ffff:13.1.68.3
--addr_to_fulluncompressed --uppercase ::ffff:13.1.68.3			=0000:0000:0000:0000:0000:FFFF:13.1.68.3
--addr_to_ifinet6 3ffe:ffff:100:f101::1					=3ffeffff0100f1010000000000000001 00
--addr_to_ifinet6 3ffe:ffff:100:f101::1/64				=3ffeffff0100f1010000000000000001 00 40
--ifinet6_to_compressed 3ffeffff0100f1010000000000000001		=3ffe:ffff:100:f101::1
--ifinet6_to_compressed 3ffeffff0100f1010000000000000001 40		=3ffe:ffff:100:f101::1/64
--addr_to_base85 1080:0:0:0:8:800:200c:417a				=4)+k&C#VzJ4br>0wv%Yp
--intype ipv6 --outtype base85 1080:0:0:0:8:800:200c:417a		=4)+k&C#VzJ4br>0wv%Yp
--base85_to_addr 4)+k&C#VzJ4br>0wv%Yp					=1080:0:0:0:8:800:200c:417a
--mac_to_eui64 00:50:BF:06:B4:F5					=250:bfff:fe06:b4f5
--mac_to_eui64 00:0:F:6:4:5						=200:fff:fe06:405
--addr_to_uncompressed --printsuffix ::ffff:1.2.3.4/64			=0:ffff:1.2.3.4
--addr_to_uncompressed --printsuffix ::ffff:1.2.3.4/63			=0:0:ffff:1.2.3.4
--addr_to_uncompressed --printsuffix 3ffe:ffff:100:f101::1/64		=0:0:0:1
--addr_to_uncompressed --printprefix 3ffe:ffff:100:f101::1/64		=3ffe:ffff:100:f101
--addr_to_uncompressed --printprefix 3ffe:ffff:100:f101::1/65		=3ffe:ffff:100:f101:0
--eui64_to_privacy 0123456789abcdef 0123456789abcdef			=4462bdea8654776d 486072ff7074945e
--addr_to_ip6arpa 3ffe::1/64						=0.0.0.0.0.0.0.0.0.0.0.0.e.f.f.3.ip6.arpa.
--intype ipv6 --outtype revnibbles.arpa	3ffe::1/64			=0.0.0.0.0.0.0.0.0.0.0.0.e.f.f.3.ip6.arpa.
-a --uppercase 3ffe::1/64						=0.0.0.0.0.0.0.0.0.0.0.0.E.F.F.3.IP6.ARPA.
--addr_to_bitstring 3ffe:ffff::1					=\\\\[x3ffeffff000000000000000000000001/128].ip6.arpa.
--intype ipv6 --outtype bitstring 3ffe:ffff::1				=\\\\[x3ffeffff000000000000000000000001/128].ip6.arpa.
--addr_to_bitstring 3ffe:ffff::1/64					=\\\\[x3ffeffff000000000000000000000001/128].ip6.arpa.
--addr_to_bitstring --printsuffix 3ffe:ffff::1/64			=\\\\[x0000000000000001/64]
--addr_to_bitstring --printprefix 3ffe:ffff::1/64			=\\\\[x3ffeffff00000000/64].ip6.arpa.
--addr_to_bitstring --printprefix --printstart 17 3ffe:ffff:0100:e100:0123:4567:89ab:cdef/64	=\\\\[xffff0100e100/48]
--addr_to_bitstring --uppercase 3ffe:ffff::1				=\\\\[x3FFEFFFF000000000000000000000001/128].IP6.ARPA.
--ipv4_to_6to4addr 11.12.13.14				=2002:b0c:d0e::
END
}

#set -x
## main ##
echo "Run 'ipv6calc' function tests..."

testscenarios | while read line; do
	# extract result
	command="`echo $line | awk -F= '{ print $1 }' | sed 's/\W*$//g'`"
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
