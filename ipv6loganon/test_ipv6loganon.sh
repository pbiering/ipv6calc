#!/bin/bash
#
# Project    : ipv6calc
# File       : test_ipv6loganon.sh
# Version    : $Id: test_ipv6loganon.sh,v 1.14 2013/03/25 07:14:09 ds6peter Exp $
# Copyright  : 2007-2013 by Peter Bieringer <pb (at) bieringer.de>
#
# Test program for "ipv6loganon"

testscenarios_standard() {
# Address
cat <<END | grep -v "^#"
192.0.2.1 - - IPv4 address			=192.0.2.0 - - IPv4 address
2001:0db8:0000:0000:81c0:0f3f:c807:1455 - - IPv6 address/privacy IID	=2001:db8::a909:4941:0:7 - - IPv6 address/privacy IID
3ffe:ffff::1 - - IPv6 address/6bone/static IID	=3ffe:ffff::a909:4291:c02d:5d1d - - IPv6 address/6bone/static IID
3FFE:1a05:510:200:0:5EFE:8CAD:8108 - - ISATAP	=3ffe:1a05:510::a909:5214:48ca:d81c - - ISATAP
ff02::1:ff00:1234 - - Solicited Node		=ff02::1:ff00:0 - - Solicited Node
3ffe::1:ff00:1234 - - Solicited Node		=3ffe::1:ff00:0 - - Solicited Node
::1 - - loopback				=::1 - - loopback
::1.2.3.4 - - compatible IPv4			=::1.2.3.0 - - compatible IPv4
::ffff:1.2.3.4 - - mapped IPv4			=::ffff:1.2.3.0 - - mapped IPv4
2002:0102:0204::1 - - 6to4 local		=2002:102:200::a909:4291:c02d:5d1d - - 6to4 local
2002:50b5:7940::50b5:7940 - - 6to4 Microsoft	=2002:50b5:7900::50b5:7900 - - 6to4 Microsoft
fe80::210:a4ff:fe01:2345 - - link local EUI-48	=fe80::a909:4291:4001:a48 - - link local EUI-48
fe80::210:a489:ab01:2345 - - link local EUI-64	=fe80::a909:4291:6001:a43 - - link local EUI-64
3ffe:ffff::210:a4ff:fe01:2345 - - 6bone EUI-48	=3ffe:ffff::a909:4291:4001:a48 - - 6bone EUI-48
3ffe:ffff::210:a489:ab01:2345 - - 6bone EUI-64	=3ffe:ffff::a909:4291:6001:a43 - - 6bone EUI-64
3ffe:ffff::000:a4ff:fe01:2345 - - 6bone EUI-48 local	=3ffe:ffff::a909:4291:4000:a43 - - 6bone EUI-48 local
3ffe:ffff::000:a489:ab01:2345 - - 6bone local	=3ffe:ffff::a909:4291:c02d:5d1d - - 6bone local
fd00:1234:5678:9abc::1 - - ULUA local		=fd00:1234:5678::a909:4291:c02d:5d1d - - ULUA local
fd00:1234:5678:9abc:210:a4ff:fe01:2345 - - ULUA EUI-64=fd00:1234:5678::a909:4291:4001:a48 - - ULUA EUI-64
fd00:1234:5678:9abc:210:a489:ab01:2345 - - ULUA EUI-64=fd00:1234:5678::a909:4291:6001:a43 - - ULUA EUI-64
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
3ffe:ffff:1234::1 - - IPv6 address/6bone/standard	=--anonymize-standard=3ffe:ffff:1234::a909:4291:c02d:5d1d - - IPv6 address/6bone/standard
3ffe:ffff:1234::1 - - IPv6 address/6bone/careful	=--anonymize-careful=3ffe:ffff:1200::a909:4291:c02d:5d1d - - IPv6 address/6bone/careful
3ffe:ffff:1234::1 - - IPv6 address/6bone/paranoid	=--anonymize-paranoid=3ffe:ffff::a909:4291:c02d:5d1d - - IPv6 address/6bone/paranoid
3FFE:1a05:510:200:0:5EFE:8CAD:8108 - - ISATAP1	=--mask-iid --anonymize-careful=3ffe:1a05:500::5efe:140.173.128.0 - - ISATAP1
3FFE:1a05:510:200:0:5EFE:8CAD:8108 - - ISATAP2	=--mask-iid --anonymize-paranoid=3ffe:1a05::5efe:140.173.0.0 - - ISATAP2
2002:50b5:7940::50b5:7940 - - 6to4 Microsoft1	=--anonymize-careful=2002:50b5:7000::50b5:7000 - - 6to4 Microsoft1
2002:50b5:7940::50b5:7940 - - 6to4 Microsoft2	=--anonymize-paranoid=2002:50b5::50b5:0 - - 6to4 Microsoft2
fe80::210:a4ff:fe01:2345 - - link local EUI-48/careful	=--anonymize-careful=fe80::a909:4291:4001:a48 - - link local EUI-48/careful
fe80::210:a489:ab01:2345 - - link local EUI-64/paranoid	=--anonymize-paranoid=fe80::a909:4291:6001:a43 - - link local EUI-64/paranoid
fec0::1234:210:a4ff:fe01:2345 - - link local EUI-48/standard	=--anonymize-standard=fec0::a909:4291:4001:a48 - - link local EUI-48/standard
fec0::1234:210:a4ff:fe01:2345 - - link local EUI-48/careful	=--anonymize-careful=fec0::a909:4291:4001:a48 - - link local EUI-48/careful
fed0::1234:210:a489:ab01:2345 - - link local EUI-64/paranoid	=--anonymize-paranoid=fed0::a909:4291:6001:a43 - - link local EUI-64/paranoid
fed0::1234:210:a489:ab01:2345 - - link local EUI-64	=--mask-ipv6 4=fec0::a909:4291:6001:a43 - - link local EUI-64
fd00:1234:5678:9abc::1 - - ULUA local		=--anonymize-standard=fd00:1234:5678::a909:4291:c02d:5d1d - - ULUA local
fd00:1234:5678:9abc:210:a4ff:fe01:2345 - - ULUA EUI-64=--anonymize-careful=fd00:1234:5600::a909:4291:4001:a48 - - ULUA EUI-64
fd00:1234:5678:9abc:210:a489:ab01:2345 - - ULUA EUI-64=--anonymize-paranoid=fd00:1234::a909:4291:6001:a43 - - ULUA EUI-64
fd00:1234:5678:9abc:210:a489:ab01:2345 - - ULUA EUI-64=--mask-ipv6 56=fd00:1234:5678:9a00:a909:4291:6001:a43 - - ULUA EUI-64
fd00:1234:5678:9abc:210:a489:ab01:2345 - - ULUA EUI-64=--mask-ipv6 48=fd00:1234:5678::a909:4291:6001:a43 - - ULUA EUI-64
fd00:1234:5678:9abc:210:a489:ab01:2345 - - ULUA EUI-64=--mask-ipv6 40=fd00:1234:5600::a909:4291:6001:a43 - - ULUA EUI-64
fd00:1234:5678:9abc:210:a489:ab01:2345 - - ULUA EUI-64=--mask-ipv6 32=fd00:1234::a909:4291:6001:a43 - - ULUA EUI-64
fd00:1234:5678:9abc:210:a489:ab01:2345 - - ULUA EUI-64=--mask-ipv6 24=fd00:1200::a909:4291:6001:a43 - - ULUA EUI-64
fd00:1234:5678:9abc:210:a489:ab01:2345 - - ULUA EUI-64=--mask-ipv6 16=fd00::a909:4291:6001:a43 - - ULUA EUI-64
fd00:1234:5678:9abc:210:a489:ab01:2345 - - ULUA EUI-64=--mask-ipv6 8=fd00::a909:4291:6001:a43 - - ULUA EUI-64
fd00:1234:5678:9abc:210:a489:ab01:2345 - - ULUA EUI-64=--mask-ipv6 4=fd00::a909:4291:6001:a43 - - ULUA EUI-64
fd00:1234:5678:9abc:210:a489:ab01:2345 - - ULUA EUI-64=--mask-iid=fd00:1234:5678:0:210:a400:: - - ULUA EUI-64
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

echo "Run 'ipv6loganon' reliability tests..." >&2
list="`testscenarios_standard | awk '{ print $1 }'`"
list="$list
`testscenarios_special | awk '{ print $1 }'`"
sortlist="`echo "$list" | sort -u`"

for entry in $sortlist; do
	echo "DEBUG : test: $entry"
	nonanonymized="`echo "$entry" | ../ipv6logstats/ipv6logstats -q`"
	anonymized="`echo "$entry" | ./ipv6loganon | ../ipv6logstats/ipv6logstats -q`"

	entry_anon="`echo "$entry" | ./ipv6loganon`"

	if [ "$nonanonymized" != "$anonymized" ]; then
		echo "ERROR : result not equal: $entry_anon"
		echo "INFO  : non-anonymized statistics"
		echo "$nonanonymized"
		echo "INFO  : anonymized statistics"
		echo "$anonymized"
		exit 1
	fi
done

if [ $retval -eq 0 ]; then
	echo "All tests were successfully done!" >&2
fi

exit $retval
