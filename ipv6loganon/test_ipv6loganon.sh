#!/bin/sh
#
# Project    : ipv6calc
# File       : test_ipv6loganon.sh
# Version    : $Id: test_ipv6loganon.sh,v 1.8 2011/05/11 20:12:20 peter Exp $
# Copyright  : 2007-2011 by Peter Bieringer <pb (at) bieringer.de>
#
# Test program for "ipv6loganon"

testscenarios_standard() {
# Address
cat <<END | grep -v "^#"
192.0.2.1 - - IPv4 address			=192.0.2.0 - - IPv4 address
3ffe:ffff::1 - - IPv6 address/6bone test	=3ffe:ffff:: - - IPv6 address/6bone test
3FFE:1a05:510:200:0:5EFE:8CAD:8108 - - ISATAP	=3ffe:1a05:510::5efe:8cad:8100 - - ISATAP
ff02::1:ff00:1234 - - Solicited Node		=ff02::1:ff00:0 - - Solicited Node
3ffe::1:ff00:1234 - - Solicited Node		=3ffe::1:ff00:0 - - Solicited Node
::1 - - loopback				=::1 - - loopback
::1.2.3.4 - - compatible IPv4			=::1.2.3.0 - - compatible IPv4
::ffff:1.2.3.4 - - mapped IPv4			=::ffff:1.2.3.0 - - mapped IPv4
2002:0102:0204::1 - - 6to4 local		=2002:102:200:: - - 6to4 local
2002:50b5:7940::50b5:7940 - - 6to4 Microsoft	=2002:50b5:7900::50b5:7900 - - 6to4 Microsoft
fe80::210:a4ff:fe01:2345 - - link local EUI-48	=fe80::210:a4ff:fe00:0 - - link local EUI-48
fe80::210:a489:ab01:2345 - - link local EUI-64	=fe80::210:a400:0:0 - - link local EUI-64
3ffe:ffff::210:a4ff:fe01:2345 - - 6bone EUI-48	=3ffe:ffff::210:a4ff:fe00:0 - - 6bone EUI-48
3ffe:ffff::210:a489:ab01:2345 - - 6bone EUI-64	=3ffe:ffff::210:a400:0:0  - - 6bone EUI-64
3ffe:ffff::000:a4ff:fe01:2345 - - 6bone local	=3ffe:ffff::a4ff:fe00:0 - - 6bone local
3ffe:ffff::000:a489:ab01:2345 - - 6bone local	=3ffe:ffff:: - - 6bone local
fd00:1234:5678:9abc::1 - - ULUA local		=fd00:1234:5678:: - - ULUA local
fd00:1234:5678:9abc:210:a4ff:fe01:2345 - - ULUA EUI-64=fd00:1234:5678:0:210:a4ff:fe00:0 - - ULUA EUI-64
fd00:1234:5678:9abc:210:a489:ab01:2345 - - ULUA EUI-64=fd00:1234:5678:0:210:a400:: - - ULUA EUI-64
3ffe:831f:ce49:7601:8000:efff:af4a:86BF - - Teredo 6bone=3ffe:831f:ce49:7601:8000:ffff:af4a:86ff - - Teredo 6bone
2001:001a:392e:a450:2cd3:75e1:6098:8104 - - ORCHID=2001:10:: - - ORCHID
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

testscenarios_special() {
# Address
cat <<END | grep -v "^#"
192.0.2.1 - - IPv4 address			=--anonymize-careful=192.0.0.0 - - IPv4 address
192.0.130.1 - - IPv4 address			=--anonymize-careful=192.0.128.0 - - IPv4 address
192.0.2.1 - - IPv4 address			=--anonymize-paranoid=192.0.0.0 - - IPv4 address
192.0.130.1 - - IPv4 address			=--anonymize-paranoid=192.0.0.0 - - IPv4 address
192.1.130.1 - - IPv4 address			=--anonymize-paranoid=192.1.0.0 - - IPv4 address
3ffe:ffff:1234::1 - - IPv6 address/6bone test	=--anonymize-standard=3ffe:ffff:1234:: - - IPv6 address/6bone test
3ffe:ffff:1234::1 - - IPv6 address/6bone test	=--anonymize-careful=3ffe:ffff:1200:: - - IPv6 address/6bone test
3ffe:ffff:1234::1 - - IPv6 address/6bone test	=--anonymize-paranoid=3ffe:ffff:: - - IPv6 address/6bone test
3FFE:1a05:510:200:0:5EFE:8CAD:8108 - - ISATAP1	=--anonymize-careful=3ffe:1a05:500::5efe:8cad:8000 - - ISATAP1
3FFE:1a05:510:200:0:5EFE:8CAD:8108 - - ISATAP2	=--anonymize-paranoid=3ffe:1a05::5efe:8cad:0 - - ISATAP2
2002:50b5:7940::50b5:7940 - - 6to4 Microsoft1	=--anonymize-careful=2002:50b5:7000::50b5:7000 - - 6to4 Microsoft1
2002:50b5:7940::50b5:7940 - - 6to4 Microsoft2	=--anonymize-paranoid=2002:50b5::50b5:0 - - 6to4 Microsoft2
fe80::210:a4ff:fe01:2345 - - link local EUI-48	=--anonymize-careful=fe80::210:a4ff:fe00:0 - - link local EUI-48
fe80::210:a489:ab01:2345 - - link local EUI-64	=--anonymize-paranoid=fe80::210:a400:0:0 - - link local EUI-64
fec0::1234:210:a4ff:fe01:2345 - - link local EUI-48	=--anonymize-standard=fec0::210:a4ff:fe00:0 - - link local EUI-48
fec0::1234:210:a4ff:fe01:2345 - - link local EUI-48	=--anonymize-careful=fec0::210:a4ff:fe00:0 - - link local EUI-48
fed0::1234:210:a489:ab01:2345 - - link local EUI-64	=--anonymize-paranoid=fed0::210:a400:0:0 - - link local EUI-64
fed0::1234:210:a489:ab01:2345 - - link local EUI-64	=--mask-ipv6 4=fec0::210:a400:0:0 - - link local EUI-64
fd00:1234:5678:9abc::1 - - ULUA local		=--anonymize-standard=fd00:1234:5678:: - - ULUA local
fd00:1234:5678:9abc:210:a4ff:fe01:2345 - - ULUA EUI-64=--anonymize-careful=fd00:1234:5600:0:210:a4ff:fe00:0 - - ULUA EUI-64
fd00:1234:5678:9abc:210:a489:ab01:2345 - - ULUA EUI-64=--anonymize-paranoid=fd00:1234::210:a400:0:0 - - ULUA EUI-64
fd00:1234:5678:9abc:210:a489:ab01:2345 - - ULUA EUI-64=--mask-ipv6 56=fd00:1234:5678:9a00:210:a400:: - - ULUA EUI-64
fd00:1234:5678:9abc:210:a489:ab01:2345 - - ULUA EUI-64=--mask-ipv6 48=fd00:1234:5678:0:210:a400:: - - ULUA EUI-64
fd00:1234:5678:9abc:210:a489:ab01:2345 - - ULUA EUI-64=--mask-ipv6 40=fd00:1234:5600:0:210:a400:: - - ULUA EUI-64
fd00:1234:5678:9abc:210:a489:ab01:2345 - - ULUA EUI-64=--mask-ipv6 32=fd00:1234::210:a400:0:0 - - ULUA EUI-64
fd00:1234:5678:9abc:210:a489:ab01:2345 - - ULUA EUI-64=--mask-ipv6 24=fd00:1200::210:a400:0:0 - - ULUA EUI-64
fd00:1234:5678:9abc:210:a489:ab01:2345 - - ULUA EUI-64=--mask-ipv6 16=fd00::210:a400:0:0 - - ULUA EUI-64
fd00:1234:5678:9abc:210:a489:ab01:2345 - - ULUA EUI-64=--mask-ipv6 8=fd00::210:a400:0:0 - - ULUA EUI-64
fd00:1234:5678:9abc:210:a489:ab01:2345 - - ULUA EUI-64=--mask-ipv6 4=fd00::210:a400:0:0 - - ULUA EUI-64
END
}


#set -x
## main ##
echo "Run 'ipv6loganon' function tests..." >&2

if [ "$1" != "bulk" ]; then
	testscenarios_standard | grep -v "^#" | while read line; do

		# extract result
		input="`echo $line | awk -F= '{ print $1 }' | sed 's/\W*$//g'`"
		result="`echo $line | awk -F= '{ print $2 }'`"
		if [ -z "$result" -o -z "$input" ]; then
			echo "Something is wrong in line '$line'"
			exit 1
		fi
		echo "IN     : $input"
		echo "CHECK  : $result"
		# get result
		output="`echo "$input" | ./ipv6loganon`"
		echo "OUT    : $output"
		retval=$?
		if [ $retval -ne 0 ]; then
			echo "Error executing 'ipv6loganon'!"
			exit 1
		fi
		# Check result
		if [ "$result" != "*" ]; then
			if [ "$output" != "$result" ]; then
				echo "RESULT : fail"
				exit 1
			fi
		fi
		echo "RESULT : ok"
		echo
	done
	retval=$?

	if [ $retval -eq 0 ]; then
		# special tests
		testscenarios_special | grep -v "^#" | while read line; do

			# extract result
			input="`echo $line | awk -F= '{ print $1 }' | sed 's/\W*$//g'`"
			options="`echo $line | awk -F= '{ print $2 }'`"
			result="`echo $line | awk -F= '{ print $3 }'`"
			if [ -z "$result" -o -z "$input" -o -z "$options" ]; then
				echo "Something is wrong in line '$line'"
				exit 1
			fi
			echo "IN     : $input"
			echo "OPTIONS: $options"
			echo "CHECK  : $result"
			# get result
			output="`echo "$input" | ./ipv6loganon $options`"
			echo "OUT    : $output"
			retval=$?
			if [ $retval -ne 0 ]; then
				echo "Error executing 'ipv6loganon'!"
				exit 1
			fi
			# Check result
			if [ "$result" != "*" ]; then
				if [ "$output" != "$result" ]; then
					echo "RESULT : fail"
					exit 1
				fi
			fi
			echo "RESULT : ok"
			echo
		done
		retval=$?
	fi
	echo 
else
	shift
	echo "Test with cache"
	testscenarios_cache | grep -v "^#" | ./ipv6loganon -V
	retval=$?
	if [ $retval -ne 0 ]; then
		echo "Error executing 'ipv6loganon'!" >&2
		exit 1
	fi
	echo "Test without cache"
	testscenarios_cache | grep -v "^#" | ./ipv6loganon -n
	retval=$?
	if [ $retval -ne 0 ]; then
		echo "Error executing 'ipv6loganon'!" >&2
		exit 1
	fi
	echo
fi

if [ $retval -eq 0 ]; then
	echo "All tests were successfully done!" >&2
fi

exit $retval
