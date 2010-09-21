#!/bin/sh
#
# Project    : ipv6calc
# File       : test_ipv6calc.sh
# Version    : $Id: test_ipv6calc.sh,v 1.16 2010/09/21 19:01:16 peter Exp $
# Copyright  : 2001-2010 by Peter Bieringer <pb (at) bieringer.de>
#
# Test patterns for ipv6calc conversions

if [ ! -x ./ipv6calc ]; then
	echo "Binary './ipv6calc' missing or not executable"
	exit 1
fi

testscenarios() {
# Command								Expected result (no space between "=" and result)
cat <<END | grep -v '^#'
## ip6.int.
--addr_to_ip6int 3ffe:ffff:100:f101::1					=1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.1.0.1.f.0.0.1.0.f.f.f.f.e.f.f.3.ip6.int.
--in ipv6 --out revnibbles.int 3ffe:ffff:100:f101::1			=1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.1.0.1.f.0.0.1.0.f.f.f.f.e.f.f.3.ip6.int.
--addr_to_ip6int 3ffe:ffff:100:f101::1/64				=1.0.1.f.0.0.1.0.f.f.f.f.e.f.f.3.ip6.int.
--in revnibbles.int --out ipv6 1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.1.0.1.f.0.0.1.0.f.f.f.f.e.f.f.3.ip6.int.	=3ffe:ffff:100:f101::1/128
## ip6.arpa.
--addr_to_ip6arpa 3ffe::1/64						=0.0.0.0.0.0.0.0.0.0.0.0.e.f.f.3.ip6.arpa.
--in ipv6 --out revnibbles.arpa	3ffe::1/64				=0.0.0.0.0.0.0.0.0.0.0.0.e.f.f.3.ip6.arpa.
-a --uppercase 3ffe::1/64						=0.0.0.0.0.0.0.0.0.0.0.0.E.F.F.3.IP6.ARPA.
-a -u 3ffe::1/64							=0.0.0.0.0.0.0.0.0.0.0.0.E.F.F.3.IP6.ARPA.
--out revnibbles.arpa abcd:1234:5678:abcd:1234:1234:1234:1234/125	=0.3.2.1.4.3.2.1.4.3.2.1.4.3.2.1.d.c.b.a.8.7.6.5.4.3.2.1.d.c.b.a.ip6.arpa. 
## to uncompressed
--addr_to_compressed 3ffe:ffff:0100:f101:0000:0000:0000:0001		=3ffe:ffff:100:f101::1
--in ipv6 --out ipv6 --printcompressed 3ffe:ffff:0100:f101:0000:0000:0000:0001 =3ffe:ffff:100:f101::1
-I ipv6 -O ipv6 -C 3ffe:ffff:0100:f101:0000:0000:0000:0001 		=3ffe:ffff:100:f101::1
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
--in ipv6 --out ipv6 --printcompressed --uppercase 0:0:0:0:0:ffff:129.144.52.38		=::FFFF:129.144.52.38
## uncompressed
--addr_to_uncompressed 3ffe:ffff:100:f101::1				=3ffe:ffff:100:f101:0:0:0:1
--in ipv6 --out ipv6 --printuncompressed 3ffe:ffff:100:f101::1		=3ffe:ffff:100:f101:0:0:0:1
--in ipv6 --out ipv6 --printuncompressed 2001:db8:0:cd30:1:2:3::	=2001:db8:0:cd30:1:2:3:0
--in ipv6 --out ipv6 --printuncompressed 2001:db8:0:cd30:1::2		=2001:db8:0:cd30:1:0:0:2
--in ipv6 --out ipv6 --printuncompressed 2001:db8:0:1::2:3		=2001:db8:0:1:0:0:2:3
--in ipv6 --out ipv6 --printuncompressed 2001:db8:0:cd30:1:2::		=2001:db8:0:cd30:1:2:0:0
--in ipv6 --out ipv6 --printuncompressed 2001:db8:0:cd30:1:2::4		=2001:db8:0:cd30:1:2:0:4
--in ipv6 --out ipv6 --printuncompressed ::db8:0:cd30:1:2:3:4		=0:db8:0:cd30:1:2:3:4
--in ipv6 --out ipv6 --printuncompressed ::1:cd30:1:2:3:4		=0:0:1:cd30:1:2:3:4
--addr_to_uncompressed 3ffe:ffff:100:f101::1/64				=3ffe:ffff:100:f101:0:0:0:1/64
--addr_to_uncompressed ::13.1.68.3					=0:0:0:0:0:0:13.1.68.3
--addr_to_uncompressed ::013.01.068.0003				=0:0:0:0:0:0:13.1.68.3
--addr_to_uncompressed ::ffff:13.1.68.3					=0:0:0:0:0:ffff:13.1.68.3
--addr_to_uncompressed --maskprefix 3ffe:ffff:100:f101::1/64		=3ffe:ffff:100:f101:0:0:0:0/64
--addr_to_uncompressed --masksuffix 3ffe:ffff:100:f101:c000::1/64	=0:0:0:0:c000:0:0:1/64
--addr_to_uncompressed --uppercase ::ffff:13.1.68.3			=0:0:0:0:0:FFFF:13.1.68.3
# selecting suffix/prefix
--addr_to_uncompressed --printsuffix ::ffff:1.2.3.4/64			=0:ffff:1.2.3.4
--in ipv6 --out ipv6 --printsuffix --printuncompressed ::ffff:1.2.3.4/64			=0:ffff:1.2.3.4
--addr_to_uncompressed --printsuffix ::ffff:1.2.3.4/63			=0:0:ffff:1.2.3.4
--addr_to_uncompressed --printsuffix 3ffe:ffff:100:f101::1/64		=0:0:0:1
--addr_to_uncompressed --printprefix 3ffe:ffff:100:f101::1/64		=3ffe:ffff:100:f101
--addr_to_uncompressed --printprefix 3ffe:ffff:100:f101::1/65		=3ffe:ffff:100:f101:0
--in ipv6 --out ipv6 --printprefix --printuncompressed 3ffe:ffff:100:f101::1/65		=3ffe:ffff:100:f101:0
## full uncompressed
--addr_to_fulluncompressed 3ffe:ffff:100:f101::1			=3ffe:ffff:0100:f101:0000:0000:0000:0001
--in ipv6 --out ipv6 --printfulluncompressed 3ffe:ffff:100:f101::1 =3ffe:ffff:0100:f101:0000:0000:0000:0001
--addr_to_fulluncompressed 3ffe:ffff:100:f101::1/64			=3ffe:ffff:0100:f101:0000:0000:0000:0001/64
--addr_to_fulluncompressed ::13.1.68.3					=0000:0000:0000:0000:0000:0000:13.1.68.3
--addr_to_fulluncompressed ::ffff:13.1.68.3				=0000:0000:0000:0000:0000:ffff:13.1.68.3
--addr_to_fulluncompressed --uppercase ::ffff:13.1.68.3			=0000:0000:0000:0000:0000:FFFF:13.1.68.3
## ifinet6
--addr_to_ifinet6 3ffe:ffff:100:f101::1					=3ffeffff0100f1010000000000000001 00
--in ipv6 --out ifinet6 3ffe:ffff:100:f101::1			=3ffeffff0100f1010000000000000001 00
--addr_to_ifinet6 3ffe:ffff:100:f101::1/64				=3ffeffff0100f1010000000000000001 00 40
--in ipv6 --out ifinet6 3ffe:ffff:100:f101::1/64		=3ffeffff0100f1010000000000000001 00 40
## to compressed
--ifinet6_to_compressed 3ffeffff0100f1010000000000000001		=3ffe:ffff:100:f101::1
--in ifinet6 --out ipv6 --printcompressed 3ffeffff0100f1010000000000000001		=3ffe:ffff:100:f101::1
--ifinet6_to_compressed 3ffeffff0100f1010000000000000001 40		=3ffe:ffff:100:f101::1/64
--in ifinet6 --out ipv6 --printcompressed 3ffeffff0100f1010000000000000001 40		=3ffe:ffff:100:f101::1/64
## to base85
--addr_to_base85 1080:0:0:0:8:800:200c:417a				=4)+k&C#VzJ4br>0wv%Yp
--in ipv6 --out base85 1080:0:0:0:8:800:200c:417a		=4)+k&C#VzJ4br>0wv%Yp
--base85_to_addr 4)+k&C#VzJ4br>0wv%Yp					=1080:0:0:0:8:800:200c:417a
## MAC to EUI-64
--mac_to_eui64 00:50:BF:06:B4:F5					=250:bfff:fe06:b4f5
--mac_to_eui64 00:0:F:6:4:5						=200:fff:fe06:405
--in mac --out eui64 00:0:F:6:4:5				=200:fff:fe06:405
## Interface identifier privacy conversion
--eui64_to_privacy 0123:4567:89ab:cdef 0123456789abcdef			=4662:bdea:8654:776d 486072ff7074945e
--in iid+token --out iid+token 0123:4567:89ab:cdef 0123456789abcdef			=4662:bdea:8654:776d 486072ff7074945e
## Bitstring
--addr_to_bitstring 3ffe:ffff::1					=\\\\[x3ffeffff000000000000000000000001/128].ip6.arpa.
--in ipv6 --out bitstring 3ffe:ffff::1				=\\\\[x3ffeffff000000000000000000000001/128].ip6.arpa.
--addr_to_bitstring 3ffe:ffff::1/64					=\\\\[x3ffeffff000000000000000000000001/128].ip6.arpa.
--addr_to_bitstring --printsuffix 3ffe:ffff::1/64			=\\\\[x0000000000000001/64]
--addr_to_bitstring --printprefix 3ffe:ffff::1/64			=\\\\[x3ffeffff00000000/64].ip6.arpa.
--addr_to_bitstring --printprefix --printstart 17 3ffe:ffff:0100:e100:0123:4567:89ab:cdef/64	=\\\\[xffff0100e100/48]
--addr_to_bitstring --uppercase 3ffe:ffff::1				=\\\\[x3FFEFFFF000000000000000000000001/128].IP6.ARPA.
--in bitstring --out ipv6 \\\\[x3FFEFFFF000000000000000000000001/64].IP6.ARPA.	=3ffe:ffff::1/64
## IPv4 -> IPv6 (6to4)
--ipv4_to_6to4addr 11.12.13.14						=2002:b0c:d0e::
--in ipv4 --out ipv6 --action conv6to4 11.12.13.14			=2002:b0c:d0e::
## IPv6 -> IPv4 (6to4)
--in ipv6 --out ipv4 --action conv6to4 2002:102:304::1			=1.2.3.4
## Prefix + MAC -> IPv6
--in prefix+mac fec0:0:0:1:: 01:23:45:67:89:01				=fec0::1:323:45ff:fe67:8901
## IPv4 -> reverse
--in ipv4addr --out revipv4 1.2.3.4					=4.3.2.1.in-addr.arpa.
## Information
-i fe80::1								=*
-i -m ff02::1								=*
# Anonymization
--action anonymize 3ffe:831f:ce49:7601:8000:efff:af4a:86BF		=3ffe:831f:ce49:7601:8000:ffff:af4a:86ff
--action anonymize 3ffe:831f:ce49:7601:8000:efff:af4a:86BF --mask-ipv4 16	=3ffe:831f:ce49:7601:8000:ffff:af4a:ffff
--action anonymize 192.0.2.1						=192.0.2.0
--action anonymize 192.0.2.1 --mask-ipv4 16				=192.0.0.0
END
}

#set -x
## main ##
echo "Run 'ipv6calc' function tests..."

testscenarios | while read line; do
	if [ -z "$line" ]; then
		# end
		continue
	fi

	# extract result
	command="`echo $line | awk -F= '{ print $1 }' | sed 's/ $//g'`"
	result="`echo $line | awk -F= '{ print $2 }'`"
	if [ -z "$result" -o -z "$command" ]; then
		echo "Something is wrong in line '$line'"
		exit 1
	fi
	echo "Test './ipv6calc $command' for '$result'"
	#continue
	# get result
	output="`./ipv6calc $command`"
	retval=$?
	if [ $retval -ne 0 ]; then
		echo "Error executing 'ipv6calc'!"
		exit 1
	fi
	# Check result
	if [ "$result" != "*" ]; then
		if [ "$output" != "$result" ]; then
			echo "Result '$output' doesn't match!"
			exit 1
		fi
	fi
done || exit 1 

echo "Run 'ipv6calc' input validation tests...(empty input)"
./ipv6calc -m --in -? | while read inputformat; do
	if echo $inputformat | grep -q '+'; then
		echo "Test './ipv6calc -q --in $inputformat \"\" \"\"'"
		./ipv6calc -q --in $inputformat "" ""
		retval=$?
	else
		echo "Test './ipv6calc -q --in $inputformat \"\"'"
		./ipv6calc -q --in $inputformat ""
		retval=$?
	fi
	if [ $retval -ne 0 -a $retval -ne 1 ]; then
		echo "Error executing 'ipv6calc'!"
		exit 1
	fi
done

echo "Run 'ipv6calc' input validation tests...(too long input)"
line="`perl -e 'print "x" x300'`"
./ipv6calc -m --in -? | while read inputformat; do
	if echo $inputformat | grep -q '+'; then
		echo "Test './ipv6calc -q --in $inputformat \"$line\" \"$line\"'"
		./ipv6calc -q --in $inputformat "$line" "$line"
		retval=$?
	else
		echo "Test './ipv6calc -q --in $inputformat \"$line\"'"
		./ipv6calc -q --in $inputformat "$line"
		retval=$?
	fi
	if [ $retval -ne 0 -a $retval -ne 1 ]; then
		echo "Error executing 'ipv6calc'!"
		exit 1
	fi
done

echo "Run 'ipv6calc' input validation tests...(strange input)"
./ipv6calc -m --in -? | while read inputformat; do
	case $inputformat in
	    hex|ifinet6)
		line="`perl -e 'print "x" x32'`"
		;;
	    *)
		line="`perl -e 'print "x" x11'`"
		;;
	esac
	if echo $inputformat | grep -q '+'; then
		echo "Test './ipv6calc -q --in $inputformat \"$line\" \"$line\"'"
		./ipv6calc -q --in $inputformat "$line" "$line"
		retval=$?
	else
		echo "Test './ipv6calc -q --in $inputformat \"$line\"'"
		./ipv6calc -q --in $inputformat "$line"
		retval=$?
	fi
	if [ $retval -ne 0 -a $retval -ne 1 ]; then
		echo "Error executing 'ipv6calc'!"
		exit 1
	fi
done


retval=$?
if [ $retval -eq 0 ]; then
	echo "All tests were successfully done!"
	exit 0
else
	echo "Tests failed! (code $retval)"
	exit $retval
fi
