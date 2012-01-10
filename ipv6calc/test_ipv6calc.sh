#!/bin/sh
#
# Project    : ipv6calc
# File       : test_ipv6calc.sh
# Version    : $Id: test_ipv6calc.sh,v 1.30 2012/01/10 20:50:16 peter Exp $
# Copyright  : 2001-2011 by Peter Bieringer <pb (at) bieringer.de>
#
# Test patterns for ipv6calc conversions

if [ ! -x ./ipv6calc ]; then
	echo "Binary './ipv6calc' missing or not executable"
	exit 1
fi

# Test Scenarios for autodetection "good case"
source ./test_scenarios.sh

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
--in mac --out eui64 00:0:F:6:4:5					=200:fff:fe06:405
--mac_to_eui64 0050BF-06B4F5						=250:bfff:fe06:b4f5
--mac_to_eui64 0050BF06B4F5						=250:bfff:fe06:b4f5
## MAC to EUI-64 (autodetect mode, not supported in pipe mode)
NOPIPETEST--out eui64 00:0:F:6:4:5					=200:fff:fe06:405
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
--action anonymize --mask-ipv4 16 3ffe:831f:ce49:7601:8000:efff:af4a:86BF	=3ffe:831f:ce49:7601:8000:ffff:af4a:ffff
--action anonymize 192.0.2.1						=192.0.2.0
--action anonymize --mask-ipv4 16 192.0.2.1				=192.0.0.0
# RFC 5952 4.2.1
--in ipv6addr --out ipv6addr 2001:db8:0:0:0:0:2:1			=2001:db8::2:1
# RFC 5952 4.2.2
--in ipv6addr --out ipv6addr 2001:db8:0:1:1:1:1:1			=2001:db8:0:1:1:1:1:1
# RFC 5952 4.2.3
--in ipv6addr --out ipv6addr 2001:0:0:1:0:0:0:1			 	=2001:0:0:1::1
--in ipv6addr --out ipv6addr 2001:db8:0:0:1:0:0:1 			=2001:db8::1:0:0:1
# RFC 5952 4.2.4
--in ipv6addr --out ipv6addr 2001:DB8:0:0:1:0:0:1		 	=2001:db8::1:0:0:1
# 6rd
--action 6rd_local_prefix --6rd_prefix 2607:fedc:ff40::/43 --6rd_relay_prefix 6.230.0.0/15 6.231.32.33	=2607:fedc:ff52:210::/60
# IPv6 literal
--in ipv6literal fe80--1.IPV6-LITERAL.NET				=fe80::1
--in ipv6literal fe80--1.IPV6-liTERal.NET				=fe80::1
--in ipv6literal fe80--218-8bff-fe17-a226s4.ipv6-literal.net		=fe80::218:8bff:fe17:a226%4
--out ipv6literal -u --in ipv6addr 2001:db8::1				=2001-DB8--1.IPV6-LITERAL.NET
--out ipv6literal -U --in ipv6addr 2001:db8::1				=2001-db8-0-0-0-0-0-1.ipv6-literal.net
--out ipv6literal -F --in ipv6addr 2001:db8::1				=2001-0db8-0000-0000-0000-0000-0000-0001.ipv6-literal.net
--out ipv6literal --in ipv6addr fe80::1%0				=fe80--1s0.ipv6-literal.net
--out ipv6literal -F --in ipv6addr fe80::1%0				=fe80-0000-0000-0000-0000-0000-0000-0001s0.ipv6-literal.net
--out ipv6addr -U --in ipv6addr fe80::1%eth0				=fe80:0:0:0:0:0:0:1%eth0
--out ipv6addr -F --in ipv6addr fe80::1%eth0				=fe80:0000:0000:0000:0000:0000:0000:0001%eth0
END
}


# Test Scenarios for autodetection "bad case"
testscenarios_auto_bad() {
cat <<END | grep -v '^#'
1.2.3.r4									ipv4addr
1.2.3.4/-1									ipv4addr
1.2.3.4/33									ipv4addr
01:23:r5:67:89:01								mac
2002:102:304::r1								ipv6addr
2002:102:304::1/-1								ipv6addr
2002:102:304::1/129								ipv6addr
1.0.0.0.r.0.0.0.0.0.0.0.0.0.0.0.1.0.1.f.0.0.1.0.f.f.f.f.e.f.f.3.ip6.int.	revnibbles.int
1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.1.0.1.f.0.0.1.0.f.f.f.f.e.f.f.3.ip6.arpa.arpa.	revnibbles.arpa
0.0.0.0.01.0.0.0.0.0.0.0.0.1.0.1.f.0.0.1.0.f.f.f.f.e.f.f.3.ip6.arpa.		revnibbles.arpa
123456789									ipv4hex
fe80---1.IPV6-LITERAL.NET							ipv6literal
END
}

# Test Scenarios for pipe handling
testscenarios_pipe() {
	cat <<END
3ffe::1:ff00:1234,--in ipv6addr --out ipv6addr -h --printuncompressed,3ffe:0:0:0:0:1:ff00:1234
3ffe::1:ff00:1234,--in ipv6addr --out ipv6addr -h --printuncompressed --printprefix --forceprefix 96,3ffe:0:0:0:0:1
END

}

#set -x
## main ##
echo "Run 'ipv6calc' basic tests..."

echo "Test './ipv6calc -v'"
output="`./ipv6calc -v`"
retval=$?
if [ $retval -ne 0 ]; then
	echo "Error executing: ipv6calc -v: $output"
	exit 1
fi

echo "Test './ipv6calc -v -v'"
output="`./ipv6calc -v -v`"
retval=$?
if [ $retval -ne 0 ]; then
	echo "Error executing: ipv6calc -v -v: $output"
	exit 1
fi


echo "Run 'ipv6calc' function tests..."

testscenarios | sed 's/NOPIPETEST//' | while read line; do
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
done || exit 1

echo "Run 'ipv6calc' input autodetection tests...(good cases)"
testscenarios_auto_good | while read input dummy; do
	echo "Test './ipv6calc -q \"$input\"'"
	./ipv6calc -q "$input" >/dev/null
	retval=$?
	if [ $retval -ne 0 ]; then
		echo "Error executing 'ipv6calc'!"
		exit 1
	fi
done || exit 1

echo "Run 'ipv6calc' input autodetection tests...(bad cases)"
testscenarios_auto_bad | while read input dummy; do
	echo "Test './ipv6calc -q \"$input\"'"
	./ipv6calc -q "$input" >/dev/null
	retval=$?
	if [ $retval -eq 0 ]; then
		echo "Error executing 'ipv6calc' ($retval)!"
		exit 1
	fi
done || exit 1

echo "Run 'ipv6calc' input tests...(good cases)"
testscenarios_auto_good | while read input type; do
	echo "Test './ipv6calc --in $type -q \"$input\"'"
	./ipv6calc --in $type -q "$input" >/dev/null
	retval=$?
	if [ $retval -ne 0 ]; then
		echo "Error executing 'ipv6calc'!"
		exit 1
	fi
done || exit 1

echo "Run 'ipv6calc' pipe tests (1)"
testscenarios_pipe | while IFS="," read input arguments result; do
	echo "Test 'echo $input | ./ipv6calc $arguments | grep \"^$result\$i\"'"
	#set -x
	echo -e $input | ./ipv6calc $arguments | grep "^$result\$" >/dev/null
	retval=$?
	#set +x
	if [ $retval -ne 0 ]; then
		echo "Error executing 'ipv6calc' ($retval)!"
		exit 1
	fi
done || exit 1

echo "Run 'ipv6calc' pipe tests (2)"
# reuse original test cases
testscenarios | grep -v "^NOPIPETEST" | while read line; do
	# extract result
	command="`echo "$line" | awk -F= '{ print $1 }' | sed 's/ $//g'`"
	result="`echo "$line" | awk -F= '{ print $2 }'`"
	#echo "command=$command" >&2
	stdin="`echo "$command" | awk '{ print $NF }'`"
	options="`echo "$command" | awk '{ for (i=1; i < NF; i++) printf "%s ", $i; }'`"
	case $command in
	    *ifinet*)
		if echo "$stdin" | grep -q "^..$"; then
			stdin="`echo "$command" | awk '{ for (i=NF-1; i <= NF; i++) printf "%s ", $i; }'`"
			options="`echo "$command" | awk '{ for (i=1; i < NF-1; i++) printf "%s ", $i; }'`"
		fi
		;;
	    *eui64_to_privacy*|*iid+token*|*prefix+mac*)
		stdin="`echo "$command" | awk '{ for (i=NF-1; i <= NF; i++) printf "%s ", $i; }'`"
		options="`echo "$command" | awk '{ for (i=1; i < NF-1; i++) printf "%s ", $i; }'`"
		;;
	esac
	echo "Test 'echo $stdin | ./ipv6calc $options | grep \"^$result\$i\"'"
	#set -x
	output="`echo -e $stdin | ./ipv6calc $options`"
	retval=$?
	#set +x
	if [ $retval -ne 0 ]; then
		echo "Error executing 'ipv6calc' ($retval)!"
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

echo "Run 'ipv6calc' pipe mode input validation tests...(too long input)"
./ipv6calc -m --in -? | while read inputformat; do
	if echo $inputformat | grep -q '+'; then
		echo "Test '8192*x 8192*x | ./ipv6calc -q --in $inputformat"
		perl -e 'print "x" x8192 . " " . "y" x8192' | ./ipv6calc -q --in $inputformat
		retval=$?
	else
		echo "Test '8192*x | ./ipv6calc -q --in $inputformat"
		perl -e 'print "x" x8192' | ./ipv6calc -q --in $inputformat
		retval=$?
	fi
	if [ $retval -ne 0 -a $retval -ne 1 ]; then
		echo "Error executing 'ipv6calc'!"
		exit 1
	fi
done || exit 1


retval=$?
if [ $retval -eq 0 ]; then
	echo "All tests were successfully done!"
	exit 0
else
	echo "Tests failed! (code $retval)"
	exit $retval
fi
