#!/usr/bin/env bash
#
# Project    : ipv6calc
# File       : test_ipv6loganon.sh
# Version    : $Id$
# Copyright  : 2007-2017 by Peter Bieringer <pb (at) bieringer.de>
#
# Test program for "ipv6loganon"

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

if [ ! -x ./ipv6loganon ]; then
	echo "Binary './ipv6loganon' missing or not executable"
	exit 1
fi

# Test proper option definitions
echo "Test proper option definition"
./ipv6loganon -h >/dev/null
if [ $? -ne 1 ]; then
	echo "ERROR : something wrong in option definition"
	exit 1
fi

echo "Run 'ipv6loganon' version help test for feature tokens..."
./ipv6loganon -v -h
retval=$?
if [ $retval -ne 0 ]; then
        echo "Error executing 'ipv6loganon -v -h'!"
        exit 1
fi
echo 

source ../ipv6calc/test_scenarios.sh

testscenarios_standard() {
# Address
cat <<END | grep -v "^#"
192.0.2.1 - - IPv4 address			=192.0.2.0 - - IPv4 address
2001:0db8:0000:0000:81c0:0f3f:c807:1455 - - IPv6 address/privacy IID	=2001:db8::9:a929:4941:0:c - - IPv6 address/privacy IID
3ffe:ffff::1 - - IPv6 address/6bone/static IID	=3ffe:ffff::9:a929:4291:c02d:5d15 - - IPv6 address/6bone/static IID
3FFE:1a05:510:200:0:5EFE:8CAD:8108 - - ISATAP	=3ffe:1a05:510:209:a929:5214:48ca:d81d - - ISATAP
ff02::1:ff00:1234 - - Solicited Node		=ff02::1:ff00:0 - - Solicited Node
3ffe::1:ff00:1234 - - Solicited Node		=3ffe::9:0:1:ff00:0 - - Solicited Node
::1 - - loopback				=::1 - - loopback
::2.2.3.4 - - compatible IPv4			=::2.2.3.0 - - compatible IPv4
::ffff:2.2.3.4 - - mapped IPv4			=::ffff:2.2.3.0 - - mapped IPv4
2002:0102:0204::1 - - 6to4 local		=2002:102:200:9:a929:4291:c02d:5d15 - - 6to4 local
2002:50b5:7940::50b5:7940 - - 6to4 Microsoft	=2002:50b5:7900:9:a929:4291:850b:5794 - - 6to4 Microsoft
fe80::210:a4ff:fe01:2345 - - link local EUI-48	=fe80::a909:4291:4021:a4c - - link local EUI-48
fe80::210:a489:ab01:2345 - - link local EUI-64	=fe80::a909:4291:6021:a43 - - link local EUI-64
3ffe:ffff::210:a4ff:fe01:2345 - - 6bone EUI-48	=3ffe:ffff::9:a929:4291:4021:a48 - - 6bone EUI-48
3ffe:ffff::210:a489:ab01:2345 - - 6bone EUI-64	=3ffe:ffff::9:a929:4291:6021:a44 - - 6bone EUI-64
3ffe:ffff::000:a4ff:fe01:2345 - - 6bone EUI-48 local	=3ffe:ffff::9:a929:4291:4000:a43 - - 6bone EUI-48 local
3ffe:ffff::000:a489:ab01:2345 - - 6bone local	=3ffe:ffff::9:a929:4291:c02d:5d15 - - 6bone local
fd00:1234:5678:9abc::1 - - ULUA local		=fd00:1234:5678:9a09:a929:4291:c02d:5d15 - - ULUA local
fd00:1234:5678:9abc:210:a4ff:fe01:2345 - - ULUA EUI-48=fd00:1234:5678:9a09:a929:4291:4021:a48 - - ULUA EUI-48
fd00:1234:5678:9abc:210:a489:ab01:2345 - - ULUA EUI-64=fd00:1234:5678:9a09:a929:4291:6021:a44 - - ULUA EUI-64
3ffe:831f:ce49:7601:8000:efff:af4a:86BF - - Teredo 6bone=3ffe:831f:ce49:7601:8000:ffff:af4a:86ff - - Teredo 6bone
2001:001a:392e:a450:2cd3:75e1:6098:8104 - - ORCHID=2001:19:a909:a909:a999:4843::e - - ORCHID
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
2.2.3.4 - -
2.2.3.4 - -
195.226.187.50	- - "IPv4 address"
::ffff:2.2.3.4 - -
2002:0102:0204::1 - -
fe80::210:a4ff:fe01:2345 - -
fe80::210:a489:ab01:2345 - -
3ffe:ffff::210:a4ff:fe01:2345 - -
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
3ffe:ffff:1234::1 - - IPv6 address/6bone/standard	=--anonymize-standard=3ffe:ffff:1234:9:a929:4291:c02d:5d15 - - IPv6 address/6bone/standard
3ffe:ffff:1234::1 - - IPv6 address/6bone/careful	=--anonymize-careful=3ffe:ffff:1234:a909:a949:4291:c02d:5d13 - - IPv6 address/6bone/careful
3ffe:ffff:1234::1 - - IPv6 address/6bone/paranoid	=--anonymize-paranoid=3ffe:ffff:1209:a909:a969:4291:c02d:5d1a - - IPv6 address/6bone/paranoid
2002:50b5:7940::50b5:7940 - - 6to4 Microsoft1	=--anonymize-careful=2002:50b5:7000:a909:a949:4291:850b:570f - - 6to4 Microsoft1
2002:50b5:7940::50b5:7940 - - 6to4 Microsoft2	=--anonymize-paranoid=2002:50b5:9:a909:a969:4291:850b:500c - - 6to4 Microsoft2
fe80::210:a4ff:fe01:2345 - - link local EUI-48/careful	=--anonymize-careful=fe80::a909:4291:4021:a4c - - link local EUI-48/careful
fe80::210:a489:ab01:2345 - - link local EUI-64/paranoid	=--anonymize-paranoid=fe80::a909:4291:6021:a43 - - link local EUI-64/paranoid
fec0::1234:210:a4ff:fe01:2345 - - site local EUI-48/standard	=--anonymize-standard=fec0::1209:a929:4291:4021:a48 - - site local EUI-48/standard
fec0::1234:210:a4ff:fe01:2345 - - site local EUI-48/careful	=--anonymize-careful=fec0::a909:a949:4291:4021:a46 - - site local EUI-48/careful
fed0::1234:210:a489:ab01:2345 - - site local EUI-64/paranoid	=--anonymize-paranoid=fed0::9:a909:a969:4291:6021:a4c - - site local EUI-64/paranoid
fd00:1234:5678:9abc::1 - - ULUA local		=--anonymize-standard=fd00:1234:5678:9a09:a929:4291:c02d:5d15 - - ULUA local
fd00:1234:5678:9abc:210:a4ff:fe01:2345 - - ULUA EUI-48/careful=--anonymize-careful=fd00:1234:5678:a909:a949:4291:4021:a46 - - ULUA EUI-48/careful
fd00:1234:5678:9abc:210:a489:ab01:2345 - - ULUA EUI-64/paranoid=--anonymize-paranoid=fd00:1234:5609:a909:a969:4291:6021:a4c - - ULUA EUI-64/paranoid
fd00:1234:5678:9abc:210:a489:ab01:2345 - - ULUA EUI-64=--mask-ipv6 56=fd00:1234:5678:9a09:a929:4291:6021:a44 - - ULUA EUI-64
fd00:1234:5678:9abc:210:a489:ab01:2345 - - ULUA EUI-64=--mask-ipv6 48=fd00:1234:5678:a909:a949:4291:6021:a45 - - ULUA EUI-64
fd00:1234:5678:9abc:210:a489:ab01:2345 - - ULUA EUI-64=--mask-ipv6 40=fd00:1234:5609:a909:a969:4291:6021:a4c - - ULUA EUI-64
fd00:1234:5678:9abc:210:a489:ab01:2345 - - ULUA EUI-64=--mask-ipv6 32=fd00:1234:a909:a909:a989:4291:6021:a4c - - ULUA EUI-64
fd00:1234:5678:9abc:210:a489:ab01:2345 - - ULUA EUI-64=--mask-ipv6 24=fd00:1209:a909:a909:a9a9:4291:6021:a40 - - ULUA EUI-64
fd00:1234:5678:9abc:210:a489:ab01:2345 - - ULUA EUI-64=--mask-ipv6 16=fd00:a909:a909:a909:a9c9:4291:6021:a44 - - ULUA EUI-64
END
}


#set -x
## main ##
echo "Run 'ipv6loganon' function tests..." >&2

if [ "$1" != "bulk" ]; then
	test="run 'ipv6loganon' standard tests"
	echo "INFO  : $test"
	testscenarios_standard | grep -v "^#" | while read line; do

		# extract result
		input="`echo $line | awk -F= '{ print $1 }' | sed 's/\W*$//g'`"
		result="`echo $line | awk -F= '{ print $2 }'`"
		if [ -z "$result" -o -z "$input" ]; then
			echo "Something is wrong in line '$line'"
			exit 1
		fi
		[ "$verbose" = "1" ] && echo "IN     : $input"
		[ "$verbose" = "1" ] && echo "CHECK  : $result"
		# get result
		output="`echo "$input" | ./ipv6loganon -q`"
		retval=$?
		[ "$verbose" = "1" ] && echo "OUT    : $output"
		if [ $retval -ne 0 ]; then
			echo "Error executing 'ipv6loganon'!"
			[ "$verbose" = "1" ] || echo "IN     : $input"
			[ "$verbose" = "1" ] || echo "CHECK  : $result"
			[ "$verbose" = "1" ] || echo "OUT    : $output"
			exit 1
		fi
		# Check result
		if [ "$result" != "*" ]; then
			if [ "$output" != "$result" ]; then
				[ "$verbose" = "1" ] || echo "IN     : $input"
				[ "$verbose" = "1" ] || echo "CHECK  : $result"
				[ "$verbose" = "1" ] || echo "OUT    : $output"
				echo "RESULT : fail"
				exit 1
			fi
		fi
		[ "$verbose" = "1" ] && echo "RESULT : ok"
		[ "$verbose" = "1" ] && echo
		[ "$verbose" = "1" ] || echo -n "."
	done
	retval=$?
	[ "$verbose" = "1" ] || echo
	echo "INFO  : $test successful"

	if [ $retval -eq 0 ]; then
		# special tests
		test="run 'ipv6loganon' special tests"
		echo "INFO  : $test"
		testscenarios_special | grep -v "^#" | while read line; do

			# extract result
			input="`echo $line | awk -F= '{ print $1 }' | sed 's/\W*$//g'`"
			options="`echo $line | awk -F= '{ print $2 }'`"
			result="`echo $line | awk -F= '{ print $3 }'`"
			if [ -z "$result" -o -z "$input" -o -z "$options" ]; then
				echo "Something is wrong in line '$line'"
				exit 1
			fi
			[ "$verbose" = "1" ] && echo "IN     : $input"
			[ "$verbose" = "1" ] && echo "OPTIONS: $options"
			[ "$verbose" = "1" ] && echo "CHECK  : $result"
			# get result
			output="`echo "$input" | ./ipv6loganon -q $options`"
			retval=$?
			[ "$verbose" = "1" ] && echo "OUT    : $output"
			if [ $retval -ne 0 ]; then
				echo "Error executing 'ipv6loganon'!"
				[ "$verbose" = "1" ] || echo "IN     : $input"
				[ "$verbose" = "1" ] || echo "CHECK  : $result"
				[ "$verbose" = "1" ] || echo "OUT    : $output"
				exit 1
			fi
			# Check result
			if [ "$result" != "*" ]; then
				if [ "$output" != "$result" ]; then
					[ "$verbose" = "1" ] || echo "IN     : $input"
					[ "$verbose" = "1" ] || echo "CHECK  : $result"
					[ "$verbose" = "1" ] || echo "OUT    : $output"
					echo "RESULT : fail"
					exit 1
				fi
			fi
			[ "$verbose" = "1" ] && echo "RESULT : ok"
			[ "$verbose" = "1" ] && echo
			[ "$verbose" = "1" ] || echo -n "."
		done
		retval=$?
		[ "$verbose" = "1" ] || echo
		echo "INFO  : $test successful"
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

if [ $retval -ne 0 ]; then
	echo "ERROR : function tests failed"
	exit 1
fi


run_loganon_reliability_tests() {
	if [ ! -x ../ipv6logstats/ipv6logstats ]; then
		echo "ERROR : missing binary: ../ipv6logstats/ipv6logstats (check build process)"
		return 1
	fi

	if [ ! -x ../ipv6logconv/ipv6logconv ]; then
		echo "ERROR : missing binary: ../ipv6logconv/ipv6logconv (check build process)"
		return 1
	fi

	if ! ../ipv6logstats/ipv6logstats -v | grep -qw STAT_REG; then
		echo "NOTICE: ipv6logstats misses basic database, skip tests"
		return 0
	fi

	if ! ../ipv6logconv/ipv6logconv -v | grep -qw CONV_REG; then
		echo "NOTICE: ipv6logconv misses basic database, skip tests"
		return 0
	fi

	local options="$*"

	if [ "$options" == "--anonymize-preset kp" ]; then
		list="`testscenarios_kp | ../ipv6calc/ipv6calc -E ipv4,ipv6`"
	else
		list="`testscenarios_standard | awk '{ print $1 }'`"
	fi

	if [ -z "$list" ]; then
		echo "ERROR : list empty in run_loganon_reliability_tests"
		return 1
	fi
	list="$list
	`testscenarios_special | awk '{ print $1 }'`"
	sortlist="`echo "$list" | sort -u`"

	echo "INFO  : run ipv6loganon/ipv6logstats reliability tests with options: $options" >&2
	for entry in $sortlist; do
		echo "DEBUG : test: $entry"
		nonanonymized="`echo "$entry" | ../ipv6logstats/ipv6logstats -q | grep -v "Time:"`"
		anonymized="`echo "$entry" | ./ipv6loganon -q $options | ../ipv6logstats/ipv6logstats -q | grep -v "Time:"`"

		if [ -z "$nonanonymized" ]; then
			echo "ERROR : result empty: nonanonymized"
			return 1
		fi

		if [ -z "$anonymized" ]; then
			echo "ERROR : result empty: anonymized"
			return 1
		fi

		entry_anon="`echo "$entry" | ./ipv6loganon -q $options`"

		if [ "$nonanonymized" != "$anonymized" ]; then
			echo "ERROR : result not equal: $entry_anon"
			export anonymized
			export nonanonymized
			diff -u <(echo "$nonanonymized") <(echo "$anonymized")
			echo "ERROR : result not equal between anonymized and non-anonymized ipv6logstats: $entry <-> $entry_anon (options: $options)"
			return 1
		fi
	done

	echo "INFO  : run ipv6loganon/ipv6logconv reliability tests with options: $options" >&2
	for entry in $sortlist; do
		echo "DEBUG : test: $entry"
		nonanonymized="`echo "$entry" | ../ipv6logconv/ipv6logconv -q --out any | grep -v "Time:"`"
		anonymized="`echo "$entry" | ./ipv6loganon -q $options | ../ipv6logconv/ipv6logconv -q --out any | grep -v "Time:"`"

		if [ -z "$nonanonymized" ]; then
			echo "ERROR : result empty: nonanonymized"
			return 1
		fi

		if [ -z "$anonymized" ]; then
			echo "ERROR : result empty: anonymized"
			return 1
		fi

		entry_anon="`echo "$entry" | ./ipv6loganon -q $options`"

		if [ "$nonanonymized" != "$anonymized" ]; then
			echo "ERROR : result not equal: $entry_anon"
			export anonymized
			export nonanonymized
			diff -u <(echo "$nonanonymized") <(echo "$anonymized")
			echo "ERROR : result not equal between anonymized and non-anonymized ipv6logconv: $entry_anon"
			return 1
		fi
	done
}

run_loganon_options_tests() {
	test="run 'ipv6loganon' anonymization option tests"
	echo "INFO  : $test"
	testscenarios_anonymization_options | while read line; do
		if [ -z "$line" ]; then
			continue
		fi

		options="`echo $line | awk '{ for ( i = 1; i < NF; i++) printf "%s ", $i }'`"
		input_result="`echo $line | awk '{ print $NF }'`"

		[ "$verbose" = "1" ] && echo "DEBUG : options=$options"
		[ "$verbose" = "1" ] && echo "DEBUG : input_result=$input_result"

		input=${input_result/=*/}
		result=${input_result/*=/}

		command="echo $input | ./ipv6loganon -q $options"

		result_real="`echo $input | ./ipv6loganon -q $options`"
		if [ $? -ne 0 ]; then
			echo "ERROR : command was not proper executed: $command"
			exit 1
		fi

		if [ "$result" != "$result_real" ]; then
			[ "$verbose" = "1" ] || echo
			echo "ERROR : result doesn't match on command: $command"
			echo "ERROR : result is      : $result_real"
			echo "ERROR : result expected: $result"
			exit 1
		else
			[ "$verbose" = "1" ] && echo "INFO  : $command -> test ok"
		fi
		[ "$verbose" = "1" ] || echo -n "."
	done || return 1
	[ "$verbose" = "1" ] || echo
	echo "INFO  : $test successful"
}

run_loganon_options_kp_tests() {
	if ! ./ipv6loganon -vv 2>&1 | grep -q "Country4=1 Country6=1 ASN4=1 ASN6=1"; then
		echo "NOTICE 'ipv6calc' has not required support for Country/ASN included, skip option kp tests..."
		return 0
	fi
	echo "Run 'ipv6loganon' anonymization option kp tests..."
	testscenarios_anonymization_options_kp | while read line; do
		if [ -z "$line" ]; then
			continue
		fi

		options="`echo $line | awk '{ for ( i = 1; i < NF; i++) printf "%s ", $i }'`"
		input_result="`echo $line | awk '{ print $NF }'`"

		echo "DEBUG : options=$options"
		echo "DEBUG : input_result=$input_result"

		input=${input_result/=*/}
		result=${input_result/*=/}

		command="echo $input | ./ipv6loganon -q $options"

		result_real="`echo $input | ./ipv6loganon -q $options`"
		if [ $? -ne 0 ]; then
			echo "ERROR : command was not proper executed: $command"
			exit 1
		fi

		if [ "$result" != "$result_real" ]; then
			[ "$verbose" = "1" ] || echo
			echo "ERROR : result doesn't match on command: $command"
			echo "ERROR : result is      : $result_real"
			echo "ERROR : result expected: $result"
			exit 1
		else
			echo "INFO  : $command -> test ok"
		fi
	done || return 1

	run_loganon_reliability_tests "--anonymize-preset kp"
	if [ $? -ne 0 ]; then
		return 1
	fi

	echo "INFO  : test scenario with huge amount of addresses..."
	testscenario_hugelist ipv4 | ./ipv6loganon --anonymize-preset kp >/dev/null
	if [ $? -ne 0 ]; then
		echo "ERROR : exit code <> 0"
		exit 1
	fi
	echo "INFO  : test scenario with huge amount of addresses: OK"
}


#### Main

run_loganon_reliability_tests
if [ $? -ne 0 ]; then
	echo "ERROR : run_loganon_reliability_tests failed"
	exit 1
fi

echo "INFO  : run 'ipv6loganon' option tests..." >&2
# Test Scenarios
source ../ipv6calc/test_scenarios.sh

run_loganon_options_tests
if [ $? -ne 0 ]; then
	echo "ERROR : run_loganon_options_tests failed"
	exit 1
fi

run_loganon_options_kp_tests
if [ $? -ne 0 ]; then
	echo "ERROR : run_loganon_options_kp_tests failed"
	exit 1
fi


echo "All tests were successfully done!" >&2

exit 0
